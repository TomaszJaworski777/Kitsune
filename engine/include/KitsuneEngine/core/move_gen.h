#pragma once

#include "bitboard.h"
#include "move.h"

class Board;

struct MoveGenerator {
	private:
		const Board &m_Board;
		const SideToMove m_Side;
		const SideToMove m_OppositeSide;
		const Square m_KingSquare;
		const Bitboard m_AttackMap;
		const Bitboard m_Checkers;
		const Bitboard m_KingMoveMap;

	public:
		MoveGenerator( const Board &board );

		[[nodiscard]]
		uint8_t GenerateQuietMoves( Move* moves ) const;

		[[nodiscard]]
		uint8_t GenerateNoisyMoves( Move* moves ) const;
};
