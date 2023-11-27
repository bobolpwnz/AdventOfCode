set(TARGET_BASE_NAME "aoc2022")
set(MAIN_LIBRARY_TARGET_NAME "lib${TARGET_BASE_NAME}")
set(MAIN_EXEC_TARGET_NAME ${TARGET_BASE_NAME})
set(TEST_EXEC_TARGET_NAME "${MAIN_EXEC_TARGET_NAME}_ut")

find_package(GTest REQUIRED)

set(MAIN_LIBRARY_UTIL_SOURCES "src/common/file_utils.cpp" "src/common/string_utils.cpp")
set(DAY_SOURCES "src/days/day01.cpp" 
                "src/days/day02.cpp"
                "src/days/day03.cpp"
                "src/days/day04.cpp"
                "src/days/day05.cpp"
                "src/days/day06.cpp"
                "src/days/day07.cpp"
                "src/days/day08.cpp"
                "src/days/day09.cpp"
                "src/days/day10.cpp"
                "src/days/day11.cpp"
                "src/days/day12.cpp"
                "src/days/day13.cpp"
                "src/days/day14.cpp"
                "src/days/day15.cpp"
                "src/days/day16.cpp")

add_library(${MAIN_LIBRARY_TARGET_NAME} ${MAIN_LIBRARY_UTIL_SOURCES} ${DAY_SOURCES})
add_executable(${MAIN_EXEC_TARGET_NAME} "src/main.cpp")

target_link_libraries(${MAIN_LIBRARY_TARGET_NAME} PUBLIC ${AOC_LIB_NAME})
target_include_directories(${MAIN_LIBRARY_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${MAIN_EXEC_TARGET_NAME} PUBLIC ${MAIN_LIBRARY_TARGET_NAME} ${AOC_LIB_NAME})


set(TEST_DAY_SOURCES "test/test_day01.cpp"
                     "test/test_day02.cpp"
                     "test/test_day03.cpp"
                     "test/test_day04.cpp"
                     "test/test_day05.cpp"
                     "test/test_day06.cpp"
                     "test/test_day07.cpp"
                     "test/test_day08.cpp"
                     "test/test_day09.cpp"
                     "test/test_day10.cpp"
                     "test/test_day11.cpp"
                     "test/test_day12.cpp"
                     "test/test_day13.cpp"
                     "test/test_day14.cpp"
                     "test/test_day15.cpp"
                     "test/test_day16.cpp")

add_executable(${TEST_EXEC_TARGET_NAME} "test/main.cpp" ${TEST_DAY_SOURCES})
target_link_libraries(${TEST_EXEC_TARGET_NAME} PRIVATE GTest::gtest GTest::gtest_main ${MAIN_LIBRARY_TARGET_NAME})
target_include_directories(${TEST_EXEC_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_command(
        TARGET ${MAIN_EXEC_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
