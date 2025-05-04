#pragma once

#include "square.h"
#include "../types.h"

extern const uint64_t SEEDS[793];

struct ZobristHash {
	private:
		uint64_t m_Value = 0;
	public:
		void UpdatePieceHash( const PieceType pieceType, const SideToMove pieceColor, const Square square ) {
			m_Value ^= SEEDS[( pieceType + pieceColor * 6 ) * 64 + square];
		}

		void UpdateSideToMoveHash( const SideToMove sideToMove ) {
			m_Value ^= SEEDS[768] * sideToMove;
		}

		void UpdateCastleRightsHash( const uint8_t rightsMask ) {
			m_Value ^= SEEDS[769 + rightsMask];
		}

		void UpdateEnPassantHash( const Square enPassantSquare ) {
			m_Value ^= SEEDS[785 + enPassantSquare.GetFile()];
		}

		operator uint64_t() const {
			return m_Value;
		}

		ZobristHash &operator=( const uint64_t value ) {
			m_Value = value;
			return *this;
		}
};
