#pragma once
#include "KitsuneEngine/core/bitboard.h"

struct Rays {
	public:
		[[nodiscard]]
		static Bitboard GetRay( Square from, Square to );

		[[nodiscard]]
		static Bitboard GetRayExcludeDestination( Square from, Square to );
};