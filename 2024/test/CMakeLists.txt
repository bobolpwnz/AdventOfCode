find_package(GTest REQUIRED)

set(TEST_DAY_SOURCES "test_day01.cpp"
                     "test_day02.cpp"
                     "test_day03.cpp"
                     "test_day04.cpp"
                     "test_day05.cpp")

add_executable(${AOC_2024_TEST_NAME} "main.cpp" ${TEST_DAY_SOURCES})
target_link_libraries(${AOC_2024_TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main ${AOC_2024_LIB_NAME})
target_include_directories(${AOC_2024_TEST_NAME} PRIVATE ${AOC_2024_INCLUDE_DIRS})

add_custom_command(
        TARGET ${AOC_2024_TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${AOC_2024_RESOURCE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
