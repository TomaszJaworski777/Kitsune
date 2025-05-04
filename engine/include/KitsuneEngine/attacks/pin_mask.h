#pragma once

#include "KitsuneEngine/core/bitboard.h"

class Board;

struct PinMask {
	private:
		Bitboard m_OrthographicMask;
		Bitboard m_DiagonalMask;
	public:
		PinMask() = default;

		PinMask( const Board &board );

		[[nodiscard]]
		Bitboard GetOrthographicMask() const {
			return m_OrthographicMask;
		}

		[[nodiscard]]
		Bitboard GetDiagonalMask() const {
			return m_DiagonalMask;
		}
};
