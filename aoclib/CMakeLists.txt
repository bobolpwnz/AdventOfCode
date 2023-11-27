add_library(${AOC_LIB_NAME} STATIC "source/application.cpp")

target_include_directories(${AOC_LIB_NAME}
                           PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:include>"
)
