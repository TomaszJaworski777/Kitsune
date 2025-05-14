add_library(Kitsune-Engine STATIC
        src/console_colors.cpp
        src/core/board.cpp
        src/core/bitboard.cpp
        src/core/move.cpp
        src/core/fen.cpp
        src/core/zobrist_hash.cpp
        src/core/attacks/rays_arrays.h
        src/core/attacks/attacks.cpp
        src/core/attacks/bishop_attacks.h
        src/core/attacks/rook_attacks.h
        src/core/attacks/knight_attacks.h
        src/core/attacks/king_attacks.h
        src/core/attacks/pawn_attacks.h
        src/core/attacks/pin_mask.cpp
        src/core/move_gen.cpp
        src/core/perft.cpp
)

target_include_directories(Kitsune-Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_options(Kitsune-Engine
        PRIVATE
        /arch:AVX2
        /GL                 # Enables whole program optimization
        /Gy                 # Enables function-level linking
        /Oi                 # Enables replacing functions with intrinsics
        /Ot                 # Favors speed when optimizing
        /Ob3                # Enables aggressive inlining
        /QIntel-jcc-erratum # Enables performance mitigations for Intel JCC erratum microcode update
        /Gw                 # Enables packaging of global data in COMDAT sections
        /GA                 # Enables generation of more optimal code for TLS storage access
        /EHs-c-             # Disables exceptions
        /GR-                # Disables RTTI
)