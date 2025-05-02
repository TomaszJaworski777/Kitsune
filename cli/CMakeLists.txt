add_executable(Kitsune-CLI
        src/main.cpp
)

target_link_libraries(Kitsune-CLI PRIVATE Kitsune-Engine)

target_include_directories(Kitsune-CLI
        PRIVATE ${CMAKE_SOURCE_DIR}/engine/include
)