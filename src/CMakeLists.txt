# CMake entry point

cmake_minimum_required (VERSION 3.0)

project (mb4d_gui)


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


# Include Boost library

set(Boost_NO_SYSTEM_PATHS TRUE)
set(BOOST_ROOT external/boost_1_65_1)
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}")
set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")



include_directories(

  ${BOOST_INCLUDE_DIRS}

  external/glfw-3.2.1/include/GLFW/

  external/glew-2.1.0/include/

  external/nuklear-2.00.0/

  external/rand48/

  .

)



set(ALL_LIBS

  ${OPENGL_LIBRARY}

  glfw

  GLEW_2100

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



add_executable(mb4d_gui

  mb4d_gui/main.cpp
  mb4d_gui/test_one_wnd.cpp
  mb4d_gui/test_two_wnd.cpp
  mb4d_gui/drawing.cpp
  mb4d_gui/drawing2.cpp
  mb4d_gui/draw_mandel_wnd.cpp
  mb4d_gui/mcamera.cpp
  mb4d_gui/parallel.cpp
  mb4d_gui/app_state.cpp
  mb4d_gui/test64bit.cpp
  mb4d_gui/test_opencl.cpp
  mb4d_gui/threads_test.cpp
  mb4d_gui/generate_fractal.cpp
  mb4d_gui/utils.cpp
  mb4d_gui/color.cpp
  mb4d_gui/distance_estimators.cpp
  mb4d_gui/work_queue.cpp
  mb4d_gui/work_queue_item.cpp

  mb4d_gui/de_params/simple_mandelbulb_de_params.cpp

)

target_link_libraries(mb4d_gui

  ${ALL_LIBS}

)

# Xcode and Visual working directories

set_target_properties(mb4d_gui PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mb4d_gui/")



SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )



if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(

  TARGET mb4d_gui POST_BUILD

  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/mb4d_gui${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/mb4d_gui/"

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
