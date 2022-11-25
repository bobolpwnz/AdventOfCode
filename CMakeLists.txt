cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_CACHE_DEFAULT FALSE)
set(ENABLE_DEVELOPER_MODE TRUE)

include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.20.0.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

project(advent_of_code_2022)

dynamic_project_options(
    PCH_HEADERS
    <vector>
    <string>
    ENABLE_CLANG_TIDY
    ENABLE_SANITIZER_ADDRESS
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

target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_subdirectory(src)

if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()
