#pragma once

#include "bitboard.h"
#include "castle_rules.h"
#include "move.h"
#include "zobrist_hash.h"
#include "../types.h"

struct FEN;

class Board {
	private:
		Bitboard m_Occupancy[2];
		Bitboard m_Pieces[6];
		ZobristHash m_Hash;
		SideToMove m_Side;
		uint8_t m_CastleRights;
		Square m_enPassantSquare;
		uint8_t m_HalfMoves;
		Square m_Rooks[4];
		uint8_t m_Phase;
		bool m_Chess960;

	public:
		Board();

		Board( const FEN &fen );

		[[nodiscard]]
		constexpr uint64_t GetHash() const {
			ZobristHash result = m_Hash;

			if ( m_enPassantSquare != Square( NULL_SQUARE ) ) {
				result.UpdateEnPassantHash( m_enPassantSquare );
			}

			result.UpdateSideToMoveHash( m_Side );
			result.UpdateCastleRightsHash( m_CastleRights );

			return result;
		}

		[[nodiscard]]
		constexpr SideToMove GetSideToMove() const {
			return m_Side;
		}

		[[nodiscard]]
		constexpr bool CanCastle( const CastleRightsFlag flag ) const {
			return m_CastleRights & flag;
		}

		[[nodiscard]]
		constexpr Square GetEnPassantSquare() const {
			return m_enPassantSquare;
		}

		[[nodiscard]]
		constexpr uint8_t GetHalfMoves() const {
			return m_HalfMoves;
		}

		[[nodiscard]]
		constexpr Square GetPhase() const {
			return m_Phase;
		}

		[[nodiscard]]
		constexpr bool GetChess960() const {
			return m_Chess960;
		}

		[[nodiscard]]
		constexpr Bitboard GetOccupancy( const SideToMove stm ) const {
			return m_Occupancy[stm];
		}

		[[nodiscard]]
		constexpr Bitboard GetOccupancy() const {
			return m_Occupancy[0] | m_Occupancy[1];
		}

		[[nodiscard]]
		constexpr Bitboard GetPieceMask( const PieceType piece ) const {
			return m_Pieces[piece];
		}

		[[nodiscard]]
		constexpr Bitboard GetPieceMask( const PieceType piece, const SideToMove stm ) const {
			return m_Occupancy[stm] & m_Pieces[piece];
		}

		[[nodiscard]]
		constexpr Square GetKingSquare( const SideToMove stm ) const {
			return GetPieceMask( KING, stm ).Ls1bSquare();
		}

		[[nodiscard]]
		constexpr SideToMove GetPieceColorOnSquare( const Square square ) const {
			if ( m_Occupancy[BLACK].GetBit( square ) )
				return BLACK;
			return WHITE;
		}

		[[nodiscard]]
		constexpr PieceType GetPieceOnSquare( const Square square ) const {
			//TODO: Test with mailbox
			for ( int piece = PAWN; piece <= KING; piece++ ) {
				if ( m_Pieces[piece].GetBit( square ) ) {
					return static_cast<PieceType>(piece);
				}
			}

			return NULL_PIECE;
		}

		constexpr void SetPieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
			m_Occupancy[pieceColor].SetBit( square );
			m_Pieces[piece].SetBit( square );
			m_Hash.UpdatePieceHash( piece, pieceColor, square );
			m_Phase += PHASE_VALUES[piece];
		}

		constexpr void RemovePieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
			m_Occupancy[pieceColor].PopBit( square );
			m_Pieces[piece].PopBit( square );
			m_Hash.UpdatePieceHash( piece, pieceColor, square );
			m_Phase -= PHASE_VALUES[piece];
		}

		[[nodiscard]]
		constexpr CastleRules GetCastleRules() const {
			return CastleRules( GetPieceMask( ROOK, WHITE ), GetPieceMask( ROOK, BLACK ),
			               GetKingSquare( WHITE ), GetKingSquare( BLACK ) );
		}

		[[nodiscard]]
		bool IsInsufficientMaterial() const;

		[[nodiscard]]
		std::string ToString() const;

		[[nodiscard]]
		std::string ToFEN() const;

		constexpr void MakeMove( const Move &move, const CastleRules &castleRules ) {
			if ( m_Side == WHITE ) {
				MakeMove_Side<WHITE>( move, castleRules );
			} else {
				MakeMove_Side<BLACK>( move, castleRules );
			}
		}

	private:
		template<SideToMove SIDE>
		constexpr void MakeMove_Side( const Move &move, const CastleRules &castleRules ) {
			switch ( move.GetFlag() ) {
				case QUIET_MOVE_FLAG: MakeMove_Flag<SIDE, QUIET_MOVE_FLAG>( move, castleRules ); break;
				case DOUBLE_PUSH_FLAG: MakeMove_Flag<SIDE, DOUBLE_PUSH_FLAG>( move, castleRules ); break;
				case KING_SIDE_CASTLE_FLAG: MakeMove_Flag<SIDE, KING_SIDE_CASTLE_FLAG>( move, castleRules ); break;
				case QUEEN_SIDE_CASTLE_FLAG: MakeMove_Flag<SIDE, QUEEN_SIDE_CASTLE_FLAG>( move, castleRules ); break;
				case CAPTURE_FLAG: MakeMove_Flag<SIDE, CAPTURE_FLAG>( move, castleRules ); break;
				case EN_PASSANT_FLAG: MakeMove_Flag<SIDE, EN_PASSANT_FLAG>( move, castleRules ); break;
				case KNIGHT_PROMOTION_FLAG: MakeMove_Flag<SIDE, KNIGHT_PROMOTION_FLAG>( move, castleRules ); break;
				case BISHOP_PROMOTION_FLAG: MakeMove_Flag<SIDE, BISHOP_PROMOTION_FLAG>( move, castleRules ); break;
				case ROOK_PROMOTION_FLAG: MakeMove_Flag<SIDE, ROOK_PROMOTION_FLAG>( move, castleRules ); break;
				case QUEEN_PROMOTION_FLAG: MakeMove_Flag<SIDE, QUEEN_PROMOTION_FLAG>( move, castleRules ); break;
				case KNIGHT_PROMOTION_CAPTURE_FLAG: MakeMove_Flag<SIDE, KNIGHT_PROMOTION_CAPTURE_FLAG>( move, castleRules ); break;
				case BISHOP_PROMOTION_CAPTURE_FLAG: MakeMove_Flag<SIDE, BISHOP_PROMOTION_CAPTURE_FLAG>( move, castleRules ); break;
				case ROOK_PROMOTION_CAPTURE_FLAG: MakeMove_Flag<SIDE, ROOK_PROMOTION_CAPTURE_FLAG>( move, castleRules ); break;
				case QUEEN_PROMOTION_CAPTURE_FLAG: MakeMove_Flag<SIDE, QUEEN_PROMOTION_CAPTURE_FLAG>( move, castleRules ); break;
				default: break;
			}
		}

		template<SideToMove SIDE, MoveFlag FLAG>
		constexpr void MakeMove_Flag( const Move &move, const CastleRules &castleRules )   {
			const Square fromSquare = move.GetFromSquare();
			const Square toSquare = move.GetToSquare();

			const bool isPromotion = (FLAG & KNIGHT_PROMOTION_FLAG) > 0 ;
			const bool isCastle = FLAG == KING_SIDE_CASTLE_FLAG || FLAG == QUEEN_SIDE_CASTLE_FLAG;

			const PieceType movedPiece = GetPieceOnSquare( fromSquare );
			const PieceType capturedPiece = GetPieceOnSquare( toSquare );

			if ( capturedPiece != NULL_PIECE ) {
				RemovePieceOnSquare( toSquare, capturedPiece, ~SIDE );
			}

			RemovePieceOnSquare( fromSquare, movedPiece, SIDE );
			if ( !isPromotion && !isCastle ) {
				SetPieceOnSquare( toSquare, movedPiece, SIDE );
			}

			if ( movedPiece == PAWN || (FLAG & CAPTURE_FLAG) > 0 ) {
				m_HalfMoves = 0;
			} else {
				m_HalfMoves++;
			}

			m_CastleRights &= ~( castleRules.GetMask( fromSquare ) | castleRules.GetMask( toSquare ) );
			m_enPassantSquare = NULL_SQUARE;

			const uint8_t sideFlip = 56 * SIDE;
			switch ( FLAG ) {
				case DOUBLE_PUSH_FLAG:
					m_enPassantSquare = toSquare ^ 8;
					break;
				case QUEEN_SIDE_CASTLE_FLAG:
					RemovePieceOnSquare( sideFlip, ROOK, SIDE );
					SetPieceOnSquare( sideFlip + 3, ROOK, SIDE );
					SetPieceOnSquare( sideFlip + 2, KING, SIDE );
					break;
				case KING_SIDE_CASTLE_FLAG:
					RemovePieceOnSquare( sideFlip + 7, ROOK, SIDE );
					SetPieceOnSquare( sideFlip + 5, ROOK, SIDE );
					SetPieceOnSquare( sideFlip + 6, KING, SIDE );
					break;
				case EN_PASSANT_FLAG:
					RemovePieceOnSquare( toSquare ^ 8, PAWN, ~SIDE );
					break;
				case KNIGHT_PROMOTION_FLAG:
				case BISHOP_PROMOTION_FLAG:
				case ROOK_PROMOTION_FLAG:
				case QUEEN_PROMOTION_FLAG:
				case KNIGHT_PROMOTION_CAPTURE_FLAG:
				case BISHOP_PROMOTION_CAPTURE_FLAG:
				case ROOK_PROMOTION_CAPTURE_FLAG:
				case QUEEN_PROMOTION_CAPTURE_FLAG:
					SetPieceOnSquare( toSquare, move.GetPromotionPieceType(), SIDE );
					break;
				default:
					break;
			}

			m_Side = ~SIDE;
		}
};
