add_library(Kitsune-Engine STATIC
        src/console_colors.cpp
        src/core/board.cpp
        src/core/bitboard.cpp
        src/core/move.cpp
        src/core/fen.cpp
        src/core/zobrist_hash.cpp
        src/core/attacks/rays.cpp
        src/core/attacks/attacks.cpp
        src/core/attacks/bishop_attacks.cpp
        src/core/attacks/rook_attacks.cpp
        src/core/attacks/knight_attacks.cpp
        src/core/attacks/king_attacks.cpp
        src/core/attacks/pawn_attacks.cpp
        src/core/attacks/pin_mask.cpp
        src/core/move_gen.cpp
)

target_include_directories(Kitsune-Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)