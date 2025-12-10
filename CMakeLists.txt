cmake_minimum_required(VERSION 3.14)
project(DungeonSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
)

set(MAIN_SRC "${SRC_DIR}/main.cpp")
list(REMOVE_ITEM SRC_FILES ${MAIN_SRC})

add_library(dungeon_lib STATIC ${SRC_FILES})

target_include_directories(dungeon_lib
    PUBLIC
        ${INCLUDE_DIR}
)

if(EXISTS ${MAIN_SRC})
    add_executable(dungeon_main ${MAIN_SRC})
    target_include_directories(dungeon_main PRIVATE ${INCLUDE_DIR})
    target_link_libraries(dungeon_main PRIVATE dungeon_lib pthread)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SRCS "${TEST_DIR}/*.cpp")

if(TEST_SRCS)
    add_executable(unit_tests ${TEST_SRCS})
    target_include_directories(unit_tests PRIVATE ${INCLUDE_DIR} ${TEST_DIR})
    target_link_libraries(unit_tests PRIVATE
        dungeon_lib
        GTest::gtest_main
        pthread
    )

    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()