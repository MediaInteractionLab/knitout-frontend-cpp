add_executable (carriers carriers.cpp)
add_executable (helloWorld helloWorld.cpp)
add_executable (sample sample.cpp)

target_link_libraries (carriers LINK_PUBLIC knitout)
target_link_libraries (helloWorld LINK_PUBLIC knitout)
target_link_libraries (sample LINK_PUBLIC knitout)