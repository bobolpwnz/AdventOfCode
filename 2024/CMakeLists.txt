set(AOC_2024_BASE_NAME "aoc2024")
set(AOC_2024_LIB_NAME "lib${AOC_2024_BASE_NAME}")
set(AOC_2024_EXEC_NAME ${AOC_2024_BASE_NAME})
set(AOC_2024_TEST_NAME "${AOC_2024_BASE_NAME}_ut")
set(AOC_2024_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(AOC_2024_RESOURCE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_subdirectory(source)
add_subdirectory(test)
