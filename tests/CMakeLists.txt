add_executable(Kitsune-Tests
        src/main.cpp
)

target_link_libraries(Kitsune-Tests PRIVATE Kitsune-Engine)

target_include_directories(Kitsune-Tests
        PRIVATE ${CMAKE_SOURCE_DIR}/engine/include
)