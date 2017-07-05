# CMake entry point

cmake_minimum_required (VERSION 3.0)

project (Simple_window)



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

  .

)



set(ALL_LIBS

  ${OPENGL_LIBRARY}

  glfw

  GLEW_1130

  OpenCL::OpenCL

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
