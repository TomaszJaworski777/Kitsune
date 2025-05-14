#pragma once

#if PEXT
#include <immintrin.h>
#include "../../engine/src/core/attacks/attack_arrays/bishop_attacks_pext.h"
#include "../../engine/src/core/attacks/attack_arrays/rook_attacks_pext.h"
#else
#include "../../engine/src/core/attacks/attack_arrays/bishop_attacks.h"
#include "../../engine/src/core/attacks/attack_arrays/rook_attacks.h"
#endif

#include "KitsuneEngine/types.h"
#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/square.h"

#include "../../engine/src/core/attacks/bishop_attacks.h"
#include "../../engine/src/core/attacks/king_attacks.h"
#include "../../engine/src/core/attacks/knight_attacks.h"
#include "../../engine/src/core/attacks/pawn_attacks.h"
#include "../../engine/src/core/attacks/rook_attacks.h"

class Board;

class Attacks {
	public:
		[[nodiscard]]
		static constexpr Bitboard GetBishopAttacks( const Square square, const Bitboard occupancy ) {
			const auto mask = BISHOP_MASKS[square];
#if !PEXT
			const auto magic = BISHOP_MAGICS[square];
			const auto shift = 64 - BISHOP_OCCUPANCY_COUNT[square];
			const uint64_t index = ( ( occupancy & mask ) * magic ) >> shift;
#else
			const uint64_t index = _pext_u64(occupancy, mask);
#endif
			return BISHOP_ATTACKS[square * 512 + index];
		}

		[[nodiscard]]
		static constexpr Bitboard GetRookAttacks( const Square square, const Bitboard occupancy ) {
			const auto mask = ROOK_MASKS[square];
#if !PEXT
			const auto magic = ROOK_MAGICS[square];
			const auto shift = 64 - ROOK_OCCUPANCY_COUNT[square];
			const uint64_t index = ( ( occupancy & mask ) * magic ) >> shift;
#else
			const uint64_t index = _pext_u64(occupancy, mask);
#endif
			return ROOK_ATTACKS[square * 4096 + index];
		}

		[[nodiscard]]
		static constexpr Bitboard GetPawnAttacks( const Square square, const SideToMove side ) {
			return PAWN_MOVES[side * 64 + square];
		}

		[[nodiscard]]
		static constexpr Bitboard GetKnightAttacks( const Square square ) {
			return KNIGHT_MOVES[square];
		}

		[[nodiscard]]
		static constexpr Bitboard GetKingAttacks( const Square square ) {
			return KING_MOVES[square];
		}

		[[nodiscard]]
		static bool IsInCheck( const Board &board );

		[[nodiscard]]
		static bool IsSquareAttacked( const Board &board, Square square, SideToMove defenderSide );

		[[nodiscard]]
		static bool IsSquareAttackedWithOccupancy( const Board &board, Square square, SideToMove defenderSide,
		                                           Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GenerateCheckersMask( const Board &board );

		[[nodiscard]]
		static Bitboard AllAttackersToSquare( const Board &board, Square square, SideToMove defenderSide,
		                                      Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GenerateAttackMap( const Board &board, SideToMove defenderSide );
};
