#pragma once

enum SideToMove : uint8_t {
    WHITE = 0,
    BLACK = 1,
};

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
    QUIET_MOVE = 0 << 6,
    DOUBLE_PUSH = 1 << 6,
    KING_SIDE_CASTLE = 2 << 6,
    QUEEN_SIDE_CASTLE = 3 << 6,
    CAPTURE = 4 << 6,
    EN_PASSANT = 5 << 6,
    KNIGHT_PROMOTION = 8 << 6,
    BISHOP_PROMOTION = 9 << 6,
    ROOK_PROMOTION = 10 << 6,
    QUEEN_PROMOTION = 11 << 6,
    KNIGHT_PROMOTION_CAPTURE = 12 << 6,
    BISHOP_PROMOTION_CAPTURE = 13 << 6,
    ROOK_PROMOTION_CAPTURE = 14 << 6,
    QUEEN_PROMOTION_CAPTURE = 15 << 6,
};

enum CastleRightsFlag : uint8_t {
    CASTLE_BLACK_KING = 0b0001,
    CASTLE_BLACK_QUEEN = 0b0010,
    CASTLE_WHITE_KING = 0b0100,
    CASTLE_WHITE_QUEEN = 0b1000,
};

constexpr uint8_t PHASE_VALUES[6]{ 0, 1, 1, 2, 4, 0 };
