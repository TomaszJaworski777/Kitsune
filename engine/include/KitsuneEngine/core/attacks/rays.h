#pragma once

#include "../../engine/src/core/attacks/rays_arrays.h"
#include "KitsuneEngine/core/bitboard.h"

struct Rays {
	public:
		[[nodiscard]]
		static constexpr Bitboard GetRay( const Square from, const Square to ) {
			return RAYS[from * 64 + to];
		}

		[[nodiscard]]
		static constexpr Bitboard GetRayExcludeDestination( const Square from, const Square to ) {
			return RAYS[from * 64 + to] & ~Bitboard( to );
		}

		[[nodiscard]]
		static constexpr Bitboard GetXRay(const Square from, const Square to) {
			return XRAYS[from * 64 + to];
		}
};
