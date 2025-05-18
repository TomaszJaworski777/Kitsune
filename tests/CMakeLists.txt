find_package(Catch2 3 REQUIRED)

add_executable(Kitsune-Tests standard.cpp frc.cpp)

target_link_libraries(Kitsune-Tests PRIVATE Kitsune-Engine)
target_link_libraries(Kitsune-Tests PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(Kitsune-Tests)

target_include_directories(Kitsune-Tests
        PRIVATE ${CMAKE_SOURCE_DIR}/engine/include
)
