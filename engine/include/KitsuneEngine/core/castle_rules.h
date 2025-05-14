#pragma once

#include "bitboard.h"
#include "square.h"
#include "KitsuneEngine/types.h"

struct CastleRules {
	private:
		Square m_Rooks[4];
		uint8_t m_Mask[64] {};

	public:
		constexpr CastleRules( const Bitboard whiteRooks, const Bitboard blackRooks, const Square whiteKingSquare,
		                  const Square blackKingSquare ) {
			whiteRooks.Map( [this, whiteKingSquare]( const Square rookSquare ) {
				const uint8_t index = rookSquare < whiteKingSquare ? 0 : 1;
				m_Rooks[index] = rookSquare;
			} );

			blackRooks.Map( [this, blackKingSquare]( const Square rookSquare ) {
				const uint8_t index = rookSquare < blackKingSquare ? 0 : 1;
				m_Rooks[2 + index] = rookSquare;
			} );

			m_Mask[m_Rooks[0]] = 0b1000;
			m_Mask[whiteKingSquare] = 0b1100;
			m_Mask[m_Rooks[1]] = 0b0100;
			m_Mask[m_Rooks[2]] = 0b0010;
			m_Mask[blackKingSquare] = 0b0011;
			m_Mask[m_Rooks[3]] = 0b0001;
		}

		[[nodiscard]]
		constexpr uint8_t GetMask( const Square square ) const {
			return m_Mask[square];
		}

		[[nodiscard]]
		constexpr Square GetRookSquare( const uint8_t idx ) const {
			return m_Rooks[idx];
		}
};
