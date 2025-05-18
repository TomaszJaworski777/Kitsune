#pragma once

#include "bitboard.h"
#include "square.h"
#include "KitsuneEngine/types.h"

struct CastleMask {
	private:
		uint8_t m_Mask[64]{ };

	public:
		constexpr CastleMask( const Square rooks[4], const Square whiteKingSquare,
		                      const Square blackKingSquare ) {
			if ( rooks[0] != NULL_SQUARE ) m_Mask[rooks[0]] = 0b1000;
			m_Mask[whiteKingSquare] = 0b1100;
			if ( rooks[1] != NULL_SQUARE ) m_Mask[rooks[1]] = 0b0100;

			if ( rooks[2] != NULL_SQUARE ) m_Mask[rooks[2]] = 0b0010;
			m_Mask[blackKingSquare] = 0b0011;
			if ( rooks[3] != NULL_SQUARE ) m_Mask[rooks[3]] = 0b0001;
		}

		[[nodiscard]]
		constexpr uint8_t GetMask( const Square square ) const {
			return m_Mask[square];
		}
};
