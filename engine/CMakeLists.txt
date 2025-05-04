add_library(Kitsune-Engine STATIC
        src/console_colors.cpp
        src/core/board.cpp
        src/core/bitboard.cpp
        src/core/move.cpp
        src/core/fen.cpp
        src/core/zobrist_hash.cpp
        src/attacks/rays.cpp
        src/attacks/attacks.cpp
        src/attacks/bishop_attacks.cpp
        src/attacks/rook_attacks.cpp
        src/attacks/knight_attacks.cpp
        src/attacks/king_attacks.cpp
        src/attacks/pawn_attacks.cpp
        src/attacks/pin_mask.cpp
)

target_include_directories(Kitsune-Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)