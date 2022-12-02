set(TEST_EXEC_TARGET_NAME "${MAIN_EXEC_TARGET_NAME}_ut")

add_executable(${TEST_EXEC_TARGET_NAME} "main.cpp" "test_day01.cpp" "test_day02.cpp")

find_package(GTest REQUIRED)

target_link_libraries(${TEST_EXEC_TARGET_NAME} PRIVATE GTest::gtest GTest::gtest_main ${MAIN_LIBRARY_TARGET_NAME})
target_include_directories(${TEST_EXEC_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(
        TARGET ${TEST_EXEC_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
