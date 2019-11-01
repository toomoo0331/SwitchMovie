cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#set(CMAKE_BUILD_TYPE Release)
add_definitions("-DCERES_FOUND=1")
set(CMAKE_RUNTI `ME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
            "CMake generation is not possible within the source directory!"
            "\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
            "\n "
            "\n rm CMakeCache.txt"
            "\n mkdir build"
            "\n cd build"
            "\n cmake .."
            )
endif()

#include(OpenCV3)
include(OpenCV2)
include(Boost)
include(TBB)
#include(Ceres)
#include(MathGL)
#include(dlib)
#include(RealSense2)
#include(OpenMVG)

set(PACKAGE_NAME "medical_dr")
set(PACKAGE_VERSION "1.0")

file(COPY config.ini DESTINATION ${CMAKE_BINARY_DIR}/bin/)
file(COPY config_test.ini DESTINATION ${CMAKE_BINARY_DIR}/bin/)

add_subdirectory(src)
add_subdirectory(test)