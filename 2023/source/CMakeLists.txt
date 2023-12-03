set(AOC_2023_LIB_SOURCES "day01.cpp" "day02.cpp" "day03.cpp")

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
