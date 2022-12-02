add_library(${MAIN_LIBRARY_TARGET_NAME} "common/file_utils.cpp" "day01/day01.cpp" "day02/day02.cpp")
add_executable(${MAIN_EXEC_TARGET_NAME} "main.cpp")

target_link_libraries(${MAIN_EXEC_TARGET_NAME} PRIVATE ${MAIN_LIBRARY_TARGET_NAME})
target_include_directories(${MAIN_LIBRARY_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(
        TARGET ${MAIN_EXEC_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
