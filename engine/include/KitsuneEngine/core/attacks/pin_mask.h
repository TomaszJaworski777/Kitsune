#pragma once

#include "KitsuneEngine/types.h"
#include "KitsuneEngine/core/bitboard.h"

class Board;

struct PinMask {
	private:
		Bitboard m_OrthographicMask;
		Bitboard m_DiagonalMask;
	public:
		PinMask() = default;

		PinMask( const Board &board, SideToMove defenderSide );

		[[nodiscard]]
		constexpr Bitboard GetOrthographicMask() const {
			return m_OrthographicMask;
		}

		[[nodiscard]]
		constexpr Bitboard GetDiagonalMask() const {
			return m_DiagonalMask;
		}
};
