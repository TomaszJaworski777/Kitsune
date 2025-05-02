#pragma once

#include "bitboard.h"
#include "../types.h"

class Board {
    private:
        Bitboard m_Occupancy[2];
        Bitboard m_Pieces[6];

    public:
        [[nodiscard]]
        Bitboard GetOccupancy( const SideToMove stm ) const {
            return m_Occupancy[stm];
        }

        [[nodiscard]]
        Bitboard GetPieceBoard( const PieceType piece ) const {
            return m_Pieces[piece];
        }

        [[nodiscard]]
        Bitboard GetPieceBoard( const PieceType piece, const SideToMove stm ) const {
            return m_Occupancy[stm] & m_Pieces[piece];
        }

        [[nodiscard]]
        Bitboard GetPieceColorOnSquare( const Square square ) const {

        }

        [[nodiscard]]
        Bitboard GetPieceOnSquare( const Square square ) const {

        }
};
