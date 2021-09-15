cmake_minimum_required (VERSION 2.8.11)
project (KNITOUT_FRONTEND_CPP)

add_library (knitout knitout.cpp)
target_include_directories (knitout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (samples)