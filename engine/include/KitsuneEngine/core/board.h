#pragma once

#include "bitboard.h"
#include "../../../src/core/zobrist_hash.h"
#include "../types.h"

class Board {
    private:
        Bitboard m_Occupancy[2];
        Bitboard m_Pieces[6];
        ZobristHash m_Hash;
        SideToMove m_Side = WHITE;
        uint8_t m_CastleRights = 0;
        Square m_enPassantSquare = 0;
        uint8_t m_HalfMoves = 0;
        uint8_t m_Phase = 0;
    public:
        [[nodiscard]]
        uint64_t GetHash() const {
            return m_Hash;
        }

        [[nodiscard]]
        SideToMove GetSideToMove() const {
            return m_Side;
        }

        [[nodiscard]]
        uint8_t GetCastleRights() const {
            return m_CastleRights;
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
        Bitboard GetPieceMask( const PieceType piece ) const {
            return m_Pieces[piece];
        }

        [[nodiscard]]
        Bitboard GetPieceMask( const PieceType piece, const SideToMove stm ) const {
            return m_Occupancy[stm] & m_Pieces[piece];
        }

        [[nodiscard]]
        Square GetStmKingSquare() const {
            return GetPieceMask( KING, m_Side ).Ls1bSquare();
        }

        [[nodiscard]]
        SideToMove GetPieceColorOnSquare( const Square square ) const {
            if (m_Occupancy[BLACK].GetBit(square))
                return BLACK;
            return WHITE;
        }

        [[nodiscard]]
        PieceType GetPieceOnSquare( const Square square ) const { //TODO: Test with mailbox
            for ( int piece = PAWN; piece <= KING; piece++ ) {
                if (m_Pieces[piece].GetBit(square)) {
                    return static_cast<PieceType>(piece);
                }
            }

            return NULL_PIECE;
        }

        void SetPieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
            m_Occupancy[pieceColor].SetBit(square);
            m_Pieces[piece].SetBit(square);
            m_Hash.UpdatePieceHash( piece, pieceColor, square );
        }

        void RemovePieceOnSquare( const Square square, const PieceType piece, const SideToMove pieceColor ) {
            m_Occupancy[pieceColor].PopBit(square);
            m_Pieces[piece].PopBit(square);
            m_Hash.UpdatePieceHash( piece, pieceColor, square );
        }

        [[nodiscard]]
        bool IsInsufficientMaterial() const;
};
