set(AOC_2023_LIB_SOURCES "day01.cpp"
                         "day02.cpp"
                         "day03.cpp"
                         "day04.cpp"
                         "day05.cpp"
                         "day06.cpp"
                         "day07.cpp"
                         "day08.cpp"
                         "day09.cpp"
                         "day10.cpp"
                         "day11.cpp"
                         "day12.cpp"
                         "day13.cpp"
                         "day14.cpp"
                         "day15.cpp"
                         "day16.cpp"
                         "day17.cpp"
                         "day18.cpp"
                         "day19.cpp"
                         "day20.cpp"
                         "day21.cpp"
)

add_library(${AOC_2023_LIB_NAME} ${AOC_2023_LIB_SOURCES})
target_link_libraries(${AOC_2023_LIB_NAME} PUBLIC ${AOC_LIB_NAME})
target_include_directories(${AOC_2023_LIB_NAME} PRIVATE ${AOC_2023_INCLUDE_DIRS})

add_executable(${AOC_2023_EXEC_NAME} "main.cpp")
target_link_libraries(${AOC_2023_EXEC_NAME} PUBLIC ${AOC_2023_LIB_NAME})

add_custom_command(
        TARGET ${AOC_2023_EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${AOC_2023_RESOURCE_DIRS}
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
