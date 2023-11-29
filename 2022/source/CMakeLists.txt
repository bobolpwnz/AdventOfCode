set(AOC_2022_LIB_SOURCES "days/day01.cpp"
                         "days/day02.cpp"
                         "days/day03.cpp"
                         "days/day04.cpp"
                         "days/day05.cpp"
                         "days/day06.cpp"
                         "days/day07.cpp"
                         "days/day08.cpp"
                         "days/day09.cpp"
                         "days/day10.cpp"
                         "days/day11.cpp"
                         "days/day12.cpp"
                         "days/day13.cpp"
                         "days/day14.cpp"
                         "days/day15.cpp"
                         "days/day16.cpp"
)

add_library(${AOC_2022_LIB_NAME} ${AOC_2022_LIB_SOURCES})
target_link_libraries(${AOC_2022_LIB_NAME} PUBLIC ${AOC_LIB_NAME})
target_include_directories(${AOC_2022_LIB_NAME} PRIVATE ${AOC_2022_INCLUDE_DIRS})

add_executable(${AOC_2022_EXEC_NAME} "main.cpp")
target_link_libraries(${AOC_2022_EXEC_NAME} PRIVATE ${AOC_2022_LIB_NAME})

add_custom_command(
        TARGET ${AOC_2022_EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${AOC_2022_RESOURCE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
