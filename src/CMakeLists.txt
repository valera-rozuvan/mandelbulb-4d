# CMake entry point

cmake_minimum_required (VERSION 3.0)

project (Simple_window)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



find_package(OpenGL REQUIRED)

find_package(OpenCL REQUIRED)



if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )

  message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )

endif()

if( CMAKE_SOURCE_DIR MATCHES " " )

  message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )

endif()

if( CMAKE_BINARY_DIR MATCHES " " )

  message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )

endif()



# Compile external dependencies

add_subdirectory (external)



include_directories(

  external/glfw-3.1.2/include/GLFW/

  external/glew-1.13.0/include/

  external/nuklear-1.40.0/

  external/rand48/

  .

)



set(ALL_LIBS

  ${OPENGL_LIBRARY}

  glfw

  GLEW_1130

  OpenCL::OpenCL

  Threads::Threads

)



add_definitions(

  -DTW_STATIC

  -DTW_NO_LIB_PRAGMA

  -DTW_NO_DIRECT3D

  -DGLEW_STATIC

  -D_CRT_SECURE_NO_WARNINGS

)



# Tutorial 1

add_executable(simple_window

  simple_window/main.cpp

  simple_window/test_cpp_with_header.cpp
  simple_window/test_one_wnd.cpp
  simple_window/test_two_wnd.cpp
  simple_window/drawing.cpp

)

target_link_libraries(simple_window

  ${ALL_LIBS}

)

# Xcode and Visual working directories

set_target_properties(simple_window PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/simple_window/")



SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )



if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(

  TARGET simple_window POST_BUILD

  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/simple_window${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/simple_window/"

)



elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )



endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )


if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()
