# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (KNITOUT_FRONTEND_CPP)

add_library (knitout knitout.cpp)
target_include_directories (knitout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (samples)