add_library(${AOC_LIB_NAME} STATIC "application.cpp" "file_utils.cpp" "string_utils.cpp")

target_include_directories(${AOC_LIB_NAME}
                           PUBLIC
                           "$<BUILD_INTERFACE:${AOC_LIB_INCLUDES}>"
                           "$<INSTALL_INTERFACE:include>"
)
