#pragma once

#include "KitsuneEngine/types.h"
#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/square.h"

class Board;

class Attacks {
	public:
		[[nodiscard]]
		static Bitboard GetBishopAttacks( Square square, Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GetRookAttacks( Square square, Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GetPawnAttacks( Square square, SideToMove side );

		[[nodiscard]]
		static Bitboard GetKnightAttacks( Square square );

		[[nodiscard]]
		static Bitboard GetKingAttacks( Square square );

		[[nodiscard]]
		static bool IsInCheck( const Board &board );

		[[nodiscard]]
		static bool IsSquareAttacked( const Board &board, Square square, SideToMove defenderSide );

		[[nodiscard]]
		static bool IsSquareAttackedWithOccupancy( const Board &board, Square square, SideToMove defenderSide, Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GenerateCheckersMask( const Board &board );

		[[nodiscard]]
		static Bitboard AllAttackersToSquare( const Board &board, Square square, SideToMove defenderSide, Bitboard occupancy );

		[[nodiscard]]
		static Bitboard GenerateAttackMap( const Board &board, SideToMove defenderSide );
};
