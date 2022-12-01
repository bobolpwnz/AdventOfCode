add_executable(avc2022 "main.cpp" "day01/day01.cpp")

add_custom_command(
        TARGET avc2022 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
