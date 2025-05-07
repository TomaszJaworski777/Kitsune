#pragma once

#include "bitboard.h"
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
		uint8_t m_Phase;

	public:
		Board();

		Board( const FEN &fen );

		[[nodiscard]]
		uint64_t GetHash() const {
			ZobristHash result = m_Hash;

			if ( m_enPassantSquare != Square( NULL_SQUARE ) ) {
				result.UpdateEnPassantHash( m_enPassantSquare );
			}

			result.UpdateSideToMoveHash( m_Side );
			result.UpdateCastleRightsHash( m_CastleRights );

			return result;
		}

		[[nodiscard]]
		SideToMove GetSideToMove() const {
			return m_Side;
		}

		[[nodiscard]]
		bool CanCastle( const CastleRightsFlag flag ) const {
			return m_CastleRights & flag;
		}

		[[nodiscard]]
		Square GetEnPassantSquare() const {
			return m_enPassantSquare;
		}

		[[nodiscard]]
		uint8_t GetHalfMoves() const {
			return m_HalfMoves;
		}

		[[nodiscard]]
		Square GetPhase() const {
			return m_Phase;
		}

		[[nodiscard]]
		Bitboard GetOccupancy( const SideToMove stm ) const {
			return m_Occupancy[stm];
		}

		[[nodiscard]]
		Bitboard GetOccupancy() const {
			return m_Occupancy[0] | m_Occupancy[1];
		}

		[[nodiscard]]
		Bitboard GetPieceMask( const PieceType piece ) const {
			return m_Pieces[piece];
		}

		[[nodiscard]]
		Bitboard GetPieceMask( const PieceType piece, const SideToMove stm ) const {
			return m_Occupancy[stm] & m_Pieces[piece];
		}

		[[nodiscard]]
		Square GetKingSquare( const SideToMove stm ) const {
			return GetPieceMask( KING, stm ).Ls1bSquare();
		}


		[[nodiscard]]
		Square GetStmKingSquare() const {
			return GetPieceMask( KING, m_Side ).Ls1bSquare();
		}

		[[nodiscard]]
		SideToMove GetPieceColorOnSquare( const Square square ) const {
			if ( m_Occupancy[BLACK].GetBit( square ) )
				return BLACK;
			return WHITE;
		}

		[[nodiscard]]
		PieceType GetPieceOnSquare( const Square square ) const {
			//TODO: Test with mailbox
			for ( int piece = PAWN; piece <= KING; piece++ ) {
				if ( m_Pieces[piece].GetBit( square ) ) {
					return static_cast<PieceType>(piece);
				}
			}

			return NULL_PIECE;
		}

		void SetPieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
			m_Occupancy[pieceColor].SetBit( square );
			m_Pieces[piece].SetBit( square );
			m_Hash.UpdatePieceHash( piece, pieceColor, square );
			m_Phase += PHASE_VALUES[piece];
		}

		void RemovePieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
			m_Occupancy[pieceColor].PopBit( square );
			m_Pieces[piece].PopBit( square );
			m_Hash.UpdatePieceHash( piece, pieceColor, square );
			m_Phase -= PHASE_VALUES[piece];
		}

		[[nodiscard]]
		bool IsInsufficientMaterial() const;

		[[nodiscard]]
		std::string ToString() const;

		void MakeMove( const Move &move );
};
