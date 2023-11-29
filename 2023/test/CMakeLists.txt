find_package(GTest REQUIRED)

set(TEST_DAY_SOURCES "test_day01.cpp")

add_executable(${AOC_2023_TEST_NAME} "main.cpp" ${TEST_DAY_SOURCES})
target_link_libraries(${AOC_2023_TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main ${AOC_2023_LIB_NAME})
target_include_directories(${AOC_2023_TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

