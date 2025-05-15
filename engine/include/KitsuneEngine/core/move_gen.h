#pragma once

#include <iostream>

#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "attacks/attacks.h"
#include "attacks/pin_mask.h"
#include "attacks/rays.h"

struct MoveGenerator {
	private:
		const Board &m_Board;
		const CastleRules &m_CastleRules;
		const Square m_KingSquare;
		const PinMask m_PinMask;
		const Bitboard m_AttackMap;
		const Bitboard m_Checkers;
		const Bitboard m_KingMoveMap;

	public:
		MoveGenerator( const Board &board, const CastleRules &castleRules );

		template<MoveGenMode MODE>
		uint8_t GenerateMoves( Move *moves ) const {
			return m_Board.GetSideToMove() == WHITE
				       ? GenerateMoves_Internal<MODE, WHITE>( moves )
				       : GenerateMoves_Internal<MODE, BLACK>( moves );
		}

	private:
		template<MoveGenMode MODE, SideToMove SIDE>
		uint8_t GenerateMoves_Internal( Move *moves ) const {
			const Move *start = moves;

			const auto diagPins = m_PinMask.GetDiagonalMask();
			const auto orthoPins = m_PinMask.GetOrthographicMask();
			const auto captureMap = m_Board.GetOccupancy( ~SIDE );

			const auto flippedOccupancy = ~m_Board.GetOccupancy();

			moves = GetKingMoves<MODE>( moves, flippedOccupancy, captureMap );

			if ( !m_Checkers ) {
				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetCastleMoves<SIDE>( moves );
				}

				const auto pushMap = ~m_Board.GetOccupancy();

				moves = GetPawnMoves<MODE, SIDE>( moves, pushMap, captureMap, diagPins, orthoPins );

				moves = GetPieceMoves<MODE, KNIGHT, SIDE>( moves, pushMap, captureMap, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, BISHOP, SIDE>( moves, pushMap, captureMap, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, ROOK, SIDE>( moves, pushMap, captureMap, diagPins, orthoPins );
			} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
				const auto checker = m_Checkers.Ls1bSquare();
				const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

				moves = GetPawnMoves<MODE, SIDE>( moves, pushMap, m_Checkers, diagPins, orthoPins );

				moves = GetPieceMoves<MODE, KNIGHT, SIDE>( moves, pushMap, m_Checkers, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, BISHOP, SIDE>( moves, pushMap, m_Checkers, diagPins, orthoPins );
				moves = GetPieceMoves<MODE, ROOK, SIDE>( moves, pushMap, m_Checkers, diagPins, orthoPins );
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

		template<SideToMove SIDE>
		Move* GetCastleMoves( Move *moves ) const {
			if constexpr ( SIDE == WHITE ) {
				if ( m_Board.CanCastle( CASTLE_WHITE_KING )
				     && !( Rays::GetRay( m_KingSquare, 6 ) & m_AttackMap )
				     && !( Rays::GetRayExcludeDestination( m_KingSquare, m_CastleRules.GetRookSquare( 1 ) ) & m_Board.
				           GetOccupancy() ) ) {
					*( moves++ ) = Move( m_KingSquare, 6, KING_SIDE_CASTLE_FLAG );
				}
				if ( m_Board.CanCastle( CASTLE_WHITE_QUEEN )
				     && !( Rays::GetRay( m_KingSquare, 2 ) & m_AttackMap )
				     && !( Rays::GetRayExcludeDestination( m_KingSquare, m_CastleRules.GetRookSquare( 0 ) ) & m_Board.
				           GetOccupancy() ) ) {
					*( moves++ ) = Move( m_KingSquare, 2, QUEEN_SIDE_CASTLE_FLAG );
				}
			} else {
				if ( m_Board.CanCastle( CASTLE_BLACK_KING )
				     && !( Rays::GetRay( m_KingSquare, 62 ) & m_AttackMap )
				     && !( Rays::GetRayExcludeDestination( m_KingSquare, m_CastleRules.GetRookSquare( 3 ) ) & m_Board.
				           GetOccupancy() ) ) {
					*( moves++ ) = Move( m_KingSquare, 62, KING_SIDE_CASTLE_FLAG );
				}
				if ( m_Board.CanCastle( CASTLE_BLACK_QUEEN )
				     && !( Rays::GetRay( m_KingSquare, 58 ) & m_AttackMap )
				     && !( Rays::GetRayExcludeDestination( m_KingSquare, m_CastleRules.GetRookSquare( 2 ) ) & m_Board.
				           GetOccupancy() ) ) {
					*( moves++ ) = Move( m_KingSquare, 58, QUEEN_SIDE_CASTLE_FLAG );
				}
			}

			return moves;
		}

		template<MoveGenMode MODE, SideToMove SIDE>
		Move* GetPawnMoves( Move *moves, const Bitboard pushMap, const Bitboard captureMap, const Bitboard diagPins,
		                    const Bitboard orthoPins ) const {
			Bitboard pawns = m_Board.GetPieceMask( PAWN, SIDE );

			if constexpr ( MODE & MoveGenMode::QUIET ) {
				moves = GetPawnPushMoves<SIDE>( moves, pawns & ~diagPins, pushMap, orthoPins );
			}

			if constexpr ( MODE & MoveGenMode::NOISY ) {
				if ( const Square enPassantSquare = m_Board.GetEnPassantSquare(); enPassantSquare != NULL_SQUARE ) {
					moves = GetPawnEnPassantMoves<SIDE>( moves, pawns, enPassantSquare );
				}

				pawns &= ~orthoPins;
				moves = GetPawnCaptureMoves<SIDE>( moves, pawns, captureMap, diagPins );
				pawns &= ~diagPins;
				moves = GetPawnPromotionMoves<SIDE>( moves, pawns, pushMap );
			}

			return moves;
		}

		template<SideToMove SIDE>
		Move* GetPawnPushMoves( Move *moves, Bitboard pawns, const Bitboard pushMap, const Bitboard orthoPins ) const {
			Bitboard verticalPin = orthoPins & ( orthoPins << 8 );
			verticalPin |= orthoPins >> 8;
			const Bitboard notPromotionRank = SIDE == WHITE ? ~Bitboard::RANK_7 : ~Bitboard::RANK_2;
			const Bitboard doublePushRank = SIDE == WHITE ? Bitboard::RANK_2 : Bitboard::RANK_7;

			pawns &= notPromotionRank;
			const Bitboard movablePawns = ( pawns & ~orthoPins ) | ( pawns & verticalPin );

			const Bitboard singlePushMap = SIDE == WHITE ? pushMap >> 8 : pushMap << 8;

			pawns = movablePawns & singlePushMap;
			Bitboard targets = SIDE == WHITE ? pawns << 8 : pawns >> 8;
			auto a = Bitboard( pawns ), b = Bitboard( targets );
			while ( a ) {
				*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), QUIET_MOVE_FLAG );
			}

			const Bitboard doublePushMap = SIDE == WHITE ? pushMap >> 16 : pushMap << 16;
			const Bitboard singlePushEmptyMap = SIDE == WHITE ? ~m_Board.GetOccupancy() >> 8 : ~m_Board.GetOccupancy() << 8;

			pawns = movablePawns & doublePushRank & singlePushEmptyMap & doublePushMap;
			targets = SIDE == WHITE ? pawns << 16 : pawns >> 16;
			a = Bitboard( pawns ), b = Bitboard( targets );
			while ( a ) {
				*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), DOUBLE_PUSH_FLAG );
			}

			return moves;
		}

		template<SideToMove SIDE>
		Move* GetPawnPromotionMoves( Move *moves, Bitboard pawns, const Bitboard pushMap ) const {
			const Bitboard promotionRank = SIDE == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
			pawns &= promotionRank;

			const Bitboard targets = ( SIDE == WHITE ? pawns << 8 : pawns >> 8 ) & pushMap;
			targets.Map( [&moves, this]( const Square toSquare ) {
				const Square fromSquare = SIDE == WHITE ? toSquare - 8 : toSquare + 8;
				( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_FLAG );
				( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_FLAG );
			} );

			return moves;
		}

		template<SideToMove SIDE>
		Move* GetPawnEnPassantMoves( Move *moves, Bitboard pawns, const Square enPassantSquare ) const {
			pawns &= Attacks::GetPawnAttacks( enPassantSquare, ~SIDE );

			pawns.Map( [&moves, enPassantSquare, this]( const Square fromSquare ) {
				Board temp = m_Board;
				const auto mv = Move( fromSquare, enPassantSquare, EN_PASSANT_FLAG );
				temp.MakeMove( mv, m_CastleRules );

				if ( !Attacks::IsSquareAttacked( temp, m_Board.GetKingSquare( SIDE ), SIDE ) ) {
					( *moves++ ) = mv;
				}
			} );

			return moves;
		}

		template<SideToMove SIDE>
		Move* GetPawnCaptureMoves( Move *moves, Bitboard pawns, const Bitboard captureMap, const Bitboard diagPins ) const {
			const Bitboard promotionRank = SIDE == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
			const Bitboard promotionPawns = pawns & promotionRank;
			pawns &= ~promotionRank;

			( pawns & ~diagPins ).Map( [&moves, captureMap, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, SIDE ) & captureMap;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
				} );
			} );

			( pawns & diagPins ).Map( [&moves, captureMap, diagPins, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, SIDE ) & captureMap & diagPins;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, CAPTURE_FLAG );
				} );
			} );

			( promotionPawns & ~diagPins ).Map( [&moves, captureMap, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, SIDE ) & captureMap;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_CAPTURE_FLAG );
				} );
			} );

			( promotionPawns & diagPins ).Map( [&moves, captureMap, diagPins, this]( const Square fromSquare ) {
				const Bitboard attacks = Attacks::GetPawnAttacks( fromSquare, SIDE ) & captureMap & diagPins;
				attacks.Map( [&moves, fromSquare]( const Square toSquare ) {
					( *moves++ ) = Move( fromSquare, toSquare, QUEEN_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, ROOK_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, BISHOP_PROMOTION_CAPTURE_FLAG );
					( *moves++ ) = Move( fromSquare, toSquare, KNIGHT_PROMOTION_CAPTURE_FLAG );
				} );
			} );

			return moves;
		}

		template<MoveGenMode MODE, PieceType PIECE, SideToMove SIDE>
		Move* GetPieceMoves( Move *moves, const Bitboard moveMap, const Bitboard captureMap, const Bitboard diagPins,
		                     const Bitboard orthoPins ) const {
			Bitboard pieces = Bitboard::EMPTY;

			if constexpr ( PIECE == KNIGHT ) {
				pieces = m_Board.GetPieceMask( KNIGHT, SIDE ) & ~diagPins & ~orthoPins;
			} else if constexpr ( PIECE == BISHOP ) {
				pieces = ( m_Board.GetPieceMask( BISHOP, SIDE ) | m_Board.GetPieceMask( QUEEN, SIDE ) ) & ~orthoPins;
			} else if constexpr ( PIECE == ROOK ) {
				pieces = ( m_Board.GetPieceMask( ROOK, SIDE ) | m_Board.GetPieceMask( QUEEN, SIDE ) ) & ~diagPins;
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
