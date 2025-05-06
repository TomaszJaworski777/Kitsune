#pragma once

#include <cstdint>

enum SideToMove : uint8_t {
	WHITE = 0,
	BLACK = 1,
};

static SideToMove operator~( const SideToMove stm ) {
	return static_cast<SideToMove>(1 - stm);
}

enum PieceType : uint8_t {
	PAWN = 0,
	KNIGHT = 1,
	BISHOP = 2,
	ROOK = 3,
	QUEEN = 4,
	KING = 5,
	NULL_PIECE = 6,
};

enum MoveFlag : uint16_t {
	QUIET_MOVE_FLAG = 0 << 6,
	DOUBLE_PUSH_FLAG = 1 << 6,
	KING_SIDE_CASTLE_FLAG = 2 << 6,
	QUEEN_SIDE_CASTLE_FLAG = 3 << 6,
	CAPTURE_FLAG = 4 << 6,
	EN_PASSANT_FLAG = 5 << 6,
	KNIGHT_PROMOTION_FLAG = 8 << 6,
	BISHOP_PROMOTION_FLAG = 9 << 6,
	ROOK_PROMOTION_FLAG = 10 << 6,
	QUEEN_PROMOTION_FLAG = 11 << 6,
	KNIGHT_PROMOTION_CAPTURE_FLAG = 12 << 6,
	BISHOP_PROMOTION_CAPTURE_FLAG = 13 << 6,
	ROOK_PROMOTION_CAPTURE_FLAG = 14 << 6,
	QUEEN_PROMOTION_CAPTURE_FLAG = 15 << 6,
};

enum CastleRightsFlag : uint8_t {
	CASTLE_BLACK_KING = 0b0001,
	CASTLE_BLACK_QUEEN = 0b0010,
	CASTLE_WHITE_KING = 0b0100,
	CASTLE_WHITE_QUEEN = 0b1000,
};

static constexpr uint8_t PHASE_VALUES[6]{ 0, 1, 1, 2, 4, 0 };

static constexpr uint8_t CASTLE_MASK[64]{
	0b1000, 0, 0, 0, 0b1100, 0, 0, 0b0100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0b0010, 0, 0, 0, 0b0011, 0, 0, 0b0001,
};

constexpr uint8_t MAX_MOVES = 218;
