#pragma once

#include <iostream>
#include <__msvc_ostream.hpp>

#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "attacks/attacks.h"
#include "attacks/pin_mask.h"
#include "attacks/rays.h"

struct MoveGenerator {
	private:
		const Board &m_Board;
		const SideToMove m_Side;
		const SideToMove m_OppositeSide;
		const Square m_KingSquare;
		const PinMask m_PinMask;
		const Bitboard m_AttackMap;
		const Bitboard m_Checkers;
		const Bitboard m_KingMoveMap;

	public:
		MoveGenerator( const Board &board );

		template<MoveGenMode MODE>
		uint8_t GenerateMoves( Move *moves ) const {
			return GenerateMoves_Internal<MODE>( moves );
		}

	private:
		template<MoveGenMode MODE>
		uint8_t GenerateMoves_Internal( Move *moves ) const {
			const Move *start = moves;

			const auto diagPins = m_PinMask.GetDiagonalMask();
			const auto orthoPins = m_PinMask.GetOrthographicMask();
			const auto captureMap = m_Board.GetOccupancy( m_OppositeSide );

			const auto flippedOccupancy = ~m_Board.GetOccupancy();

			moves = GetKingMoves<MODE>( moves, flippedOccupancy, captureMap );

			if ( !m_Checkers ) {
				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetCastleMoves( moves );
				}

				const auto pushMap = ~m_Board.GetOccupancy();

				moves = GetPawnMoves<MODE>( moves, pushMap, captureMap, diagPins, orthoPins );

				moves = GetPieceMoves<MODE, KNIGHT>( moves, pushMap, captureMap, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, BISHOP>( moves, pushMap, captureMap, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, ROOK>( moves, pushMap, captureMap, diagPins, orthoPins );
			} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
				const auto checker = m_Checkers.Ls1bSquare();
				const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

				moves = GetPawnMoves<MODE>( moves, pushMap, m_Checkers, diagPins, orthoPins );

				moves = GetPieceMoves<MODE, KNIGHT>( moves, pushMap, m_Checkers, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, BISHOP>( moves, pushMap, m_Checkers, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, ROOK>( moves, pushMap, m_Checkers, diagPins, orthoPins );
			}

			return static_cast<uint8_t>(moves - start);
		}

		template<MoveGenMode MODE>
		Move* GetKingMoves( Move *moves, const Bitboard flippedOccupancy, const Bitboard captureMap ) const {
			if constexpr ( MODE & MoveGenMode::QUIET ) {
				( m_KingMoveMap & flippedOccupancy ).Map( [&moves, this]( const Square square ) {
					*( moves++ ) = Move( m_KingSquare, square, QUIET_MOVE_FLAG );
				} );
			}

			if constexpr ( MODE & MoveGenMode::NOISY ) {
				( m_KingMoveMap & captureMap ).Map( [&moves, this]( const Square square ) {
					*( moves++ ) = Move( m_KingSquare, square, CAPTURE_FLAG );
				} );
			}

			return moves;
		}

		Move* GetCastleMoves( Move *moves ) const;

		template<MoveGenMode MODE>
		Move* GetPawnMoves( Move *moves, const Bitboard pushMap, const Bitboard captureMap, const Bitboard diagPins,
		                    const Bitboard orthoPins ) const {
			Bitboard pawns = m_Board.GetPieceMask( PAWN, m_Side );

			if constexpr ( MODE & MoveGenMode::QUIET ) {
				moves = GetPawnPushMoves( moves, pushMap, diagPins, orthoPins );
			}

			if constexpr ( MODE & MoveGenMode::NOISY ) {
				if ( const Square enPassantSquare = m_Board.GetEnPassantSquare(); enPassantSquare != NULL_SQUARE ) {
					moves = GetPawnEnPassantMoves( moves, pawns, enPassantSquare );
				}

				pawns &= ~orthoPins;
				moves = GetPawnCaptureMoves( moves, pawns, captureMap, diagPins );
				pawns &= ~diagPins;
				moves = GetPawnPromotionMoves( moves, pawns, pushMap );
			}

			return moves;
		}

		Move* GetPawnPushMoves( Move *moves, const Bitboard pushMap, const Bitboard diagPins, const Bitboard orthoPins ) const {
			Bitboard verticalPin = orthoPins & ( orthoPins << 8 );
			verticalPin |= orthoPins >> 8;
			const Bitboard promotionRank = m_Side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
			const Bitboard doublePushRank = m_Side == WHITE ? Bitboard::RANK_2 : Bitboard::RANK_7;
			const Bitboard pawnPool = m_Board.GetPieceMask( PAWN, m_Side ) & ~diagPins & ~promotionRank;
			const Bitboard pinnedPawns = pawnPool & verticalPin;
			const Bitboard movablePawns = ( pawnPool & ~orthoPins ) | pinnedPawns;
			const Bitboard doublePushPawns = movablePawns & doublePushRank;

			const Bitboard singlePushMap = m_Side == WHITE ? pushMap >> 8 : pushMap << 8;
			const Bitboard doublePushMap = m_Side == WHITE ? pushMap >> 16 : pushMap << 16;
			const Bitboard singlePushEmptyMap = m_Side == WHITE ? ~m_Board.GetOccupancy() >> 8 : ~m_Board.GetOccupancy() << 8;

			Bitboard pawns = movablePawns & singlePushMap;
			Bitboard targets = m_Side == WHITE ? pawns << 8 : pawns >> 8;
			auto a = Bitboard( pawns ), b = Bitboard( targets );
			while ( a ) {
				*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), QUIET_MOVE_FLAG );
			}

			pawns = doublePushPawns & singlePushEmptyMap & doublePushMap;
			targets = m_Side == WHITE ? pawns << 16 : pawns >> 16;
			a = Bitboard( pawns ), b = Bitboard( targets );
			while ( a ) {
				*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), DOUBLE_PUSH_FLAG );
			}

			return moves;
		}

		Move* GetPawnPromotionMoves( Move *moves, Bitboard pawns, const Bitboard pushMap ) const {
			const Bitboard promotionRank = m_Side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
			pawns &= promotionRank;

			const Bitboard targets = (m_Side == WHITE ? pawns << 8 : pawns >> 8) & pushMap;
			targets.Map( [&moves, this]( const Square toSquare ) {
				const Square fromSquare = m_Side == WHITE ? toSquare - 8 : toSquare + 8;
				( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_FLAG );
			});

			return moves;
		}

		Move* GetPawnEnPassantMoves( Move *moves, Bitboard pawns, const Square enPassantSquare ) const {
			pawns &= Attacks::GetPawnAttacks( enPassantSquare, m_OppositeSide );

			pawns.Map( [&moves, enPassantSquare, this]( const Square fromSquare ) {
				Board temp = m_Board;
				const auto mv = Move( fromSquare, enPassantSquare, EN_PASSANT_FLAG );
				temp.MakeMove( mv );

				if ( !Attacks::IsSquareAttacked( temp, m_Board.GetKingSquare( m_Side ), m_Side ) ) {
					( *moves++ ) = mv;
				}
			});

			return moves;
		}

		Move* GetPawnCaptureMoves( Move *moves, Bitboard pawns, const Bitboard captureMap, const Bitboard diagPins ) const {
			const Bitboard promotionRank = m_Side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
			const Bitboard promotionPawns = pawns & promotionRank;
			pawns &= ~promotionRank;

			( pawns & ~diagPins ).Map( [&moves, captureMap, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, m_Side ) & captureMap;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
				} );
			} );

			( pawns & diagPins ).Map( [&moves, captureMap, diagPins, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, m_Side ) & captureMap & diagPins;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
				} );
			} );

			( promotionPawns & ~diagPins ).Map( [&moves, captureMap, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, m_Side ) & captureMap;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_CAPTURE_FLAG );
				} );
			} );

			( promotionPawns & diagPins ).Map( [&moves, captureMap, diagPins, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, m_Side ) & captureMap & diagPins;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_CAPTURE_FLAG );
				} );
			} );

			return moves;
		}

		template<MoveGenMode MODE, PieceType PIECE>
		Move* GetPieceMoves( Move *moves, const Bitboard moveMap, const Bitboard captureMap, const Bitboard diagPins,
		                     const Bitboard orthoPins ) const {
			Bitboard pieces = Bitboard::EMPTY;

			if constexpr ( PIECE == KNIGHT ) {
				pieces = m_Board.GetPieceMask( KNIGHT, m_Side ) & ~diagPins & ~orthoPins;
			} else if constexpr ( PIECE == BISHOP ) {
				pieces = ( m_Board.GetPieceMask( BISHOP, m_Side ) | m_Board.GetPieceMask( QUEEN, m_Side ) ) & ~orthoPins;
			} else if constexpr ( PIECE == ROOK ) {
				pieces = ( m_Board.GetPieceMask( ROOK, m_Side ) | m_Board.GetPieceMask( QUEEN, m_Side ) ) & ~diagPins;
			}

			Bitboard pinnedPieces = Bitboard::EMPTY;
			if constexpr ( PIECE == BISHOP ) {
				pinnedPieces = pieces & diagPins;
			} else if constexpr ( PIECE == ROOK ) {
				pinnedPieces = pieces & orthoPins;
			}

			const Bitboard nonPinnedPieces = pieces & ~pinnedPieces;

			nonPinnedPieces.Map( [&moves, moveMap, captureMap, this]( const Square fromSquare ) {
				Bitboard attacks = Bitboard::EMPTY;
				if constexpr ( PIECE == KNIGHT ) {
					attacks = Attacks::GetKnightAttacks( fromSquare );
				} else if constexpr ( PIECE == BISHOP ) {
					attacks = Attacks::GetBishopAttacks( fromSquare, m_Board.GetOccupancy() );
				} else if constexpr ( PIECE == ROOK ) {
					attacks = Attacks::GetRookAttacks( fromSquare, m_Board.GetOccupancy() );
				}

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					( attacks & moveMap ).Map( [&moves, fromSquare]( const Square toSquare ) {
						*( moves++ ) = Move( fromSquare, toSquare, QUIET_MOVE_FLAG );
					} );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					( attacks & captureMap ).Map( [&moves, fromSquare]( const Square toSquare ) {
						*( moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
					} );
				}
			} );

			pinnedPieces.Map( [&moves, moveMap, captureMap, diagPins, orthoPins, this]( const Square fromSquare ) {
				Bitboard attacks = Bitboard::EMPTY;
				if constexpr ( PIECE == BISHOP ) {
					attacks = Attacks::GetBishopAttacks( fromSquare, m_Board.GetOccupancy() ) & diagPins;
				} else if constexpr ( PIECE == ROOK ) {
					attacks = Attacks::GetRookAttacks( fromSquare, m_Board.GetOccupancy() ) & orthoPins;
				}

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					( attacks & moveMap ).Map( [&moves, fromSquare]( const Square toSquare ) {
						*( moves++ ) = Move( fromSquare, toSquare, QUIET_MOVE_FLAG );
					} );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					( attacks & captureMap ).Map( [&moves, fromSquare]( const Square toSquare ) {
						*( moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
					} );
				}
			} );

			return moves;
		}
};
