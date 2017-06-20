cmake_minimum_required(VERSION 3.7)

project (hello)

find_package(OpenCL REQUIRED)

add_executable (hello main.cpp)
target_link_libraries(hello OpenCL::OpenCL)
