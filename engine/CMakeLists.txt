add_library(Kitsune-Engine STATIC
        src/console_colors.cpp
        src/core/board.cpp
        src/core/bitboard.cpp
        src/core/move.cpp
)

target_include_directories(Kitsune-Engine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)