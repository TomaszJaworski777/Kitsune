include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

include(CTest)
include(Catch)

add_executable(Kitsune-Tests standard.cpp frc.cpp fen.cpp)

target_link_libraries(Kitsune-Tests PRIVATE Kitsune-Engine)
target_link_libraries(Kitsune-Tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(Kitsune-Tests)

target_include_directories(Kitsune-Tests
        PRIVATE ${CMAKE_SOURCE_DIR}/engine/include
)
