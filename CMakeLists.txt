cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENABLE_CACHE_DEFAULT FALSE)
set(ENABLE_DEVELOPER_MODE
    TRUE
    CACHE BOOL "Enable 'developer mode'")
set(OPT_WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT FALSE)

include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.20.0.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

project(
  advent_of_code_2022
  VERSION 0.0.1
  DESCRIPTION "Advent of Code 2022"
  HOMEPAGE_URL "https://github.com/bobolpwnz/AdventOfCode2022"
  LANGUAGES CXX)

get_property(BUILDING_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(BUILDING_MULTI_CONFIG)
  if(NOT CMAKE_BUILD_TYPE)
    # Make sure that all supported configuration types have their
    # associated conan packages available. You can reduce this
    # list to only the configuration types you use, but only if one
    # is not forced-set on the command line for VS
    message(TRACE "Setting up multi-config build types")
    set(CMAKE_CONFIGURATION_TYPES
        Debug
        Release
        RelWithDebInfo
        MinSizeRel
        CACHE STRING "Enabled build types" FORCE)
  else()
    message(TRACE "User chose a specific build type, so we are using that")
    set(CMAKE_CONFIGURATION_TYPES
        ${CMAKE_BUILD_TYPE}
        CACHE STRING "Enabled build types" FORCE)
  endif()
endif()

include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

dynamic_project_options(
    ENABLE_CONAN
    PCH_HEADERS
    <vector>
    <string>
    MSVC_WARNINGS
    CPPCHECK_OPTIONS
    --enable=style,performance,warning,portability
    --inline-suppr
    # We cannot act on a bug/missing feature of cppcheck
    --suppress=cppcheckError
    --suppress=internalAstError
    # if a file does not have an internalAstError, we get an unmatchedSuppression error
    --suppress=unmatchedSuppression
    --suppress=passedByValue
    --suppress=syntaxError
    --inconclusive
)

if(ENABLE_DEVELOPER_MODE)
    if("${CMAKE_CXX_CLANG_TIDY_DRIVER_MODE}" STREQUAL "cl")
        set(CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY} -extra-arg=/EHsc)
    else()
        set(CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY} -extra-arg=-fexceptions)
    endif()
endif()

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

set(TARGET_BASE_NAME "aoc2022")
set(MAIN_LIBRARY_TARGET_NAME "lib${TARGET_BASE_NAME}")
set(MAIN_EXEC_TARGET_NAME ${TARGET_BASE_NAME})
set(TEST_EXEC_TARGET_NAME "${MAIN_EXEC_TARGET_NAME}_ut")

add_library(${MAIN_LIBRARY_TARGET_NAME} "src/common/file_utils.cpp" "src/common/string_utils.cpp" "src/days/day01.cpp" "src/days/day02.cpp" "src/days/day03.cpp" "src/days/day04.cpp" "src/days/day05.cpp" "src/days/day06.cpp" "src/days/day07.cpp")

add_executable(${MAIN_EXEC_TARGET_NAME} "src/main.cpp")
target_link_libraries(${MAIN_EXEC_TARGET_NAME} PRIVATE ${MAIN_LIBRARY_TARGET_NAME})
target_include_directories(${MAIN_LIBRARY_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

find_package(GTest REQUIRED)

add_executable(${TEST_EXEC_TARGET_NAME} "test/main.cpp" "test/test_day01.cpp" "test/test_day02.cpp" "test/test_day03.cpp" "test/test_day04.cpp" "test/test_day05.cpp" "test/test_day06.cpp" "test/test_day07.cpp")
target_link_libraries(${TEST_EXEC_TARGET_NAME} PRIVATE GTest::gtest GTest::gtest_main ${MAIN_LIBRARY_TARGET_NAME})
target_include_directories(${TEST_EXEC_TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(
        TARGET ${MAIN_EXEC_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources)
