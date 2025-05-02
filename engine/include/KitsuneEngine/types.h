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
