#==============================================================================
#         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
#
#          Distributed under the Boost Software License, Version 1.0.
#                 See accompanying file LICENSE.txt or copy at
#                     http://www.boost.org/LICENSE_1_0.txt
#==============================================================================


cmake_minimum_required(VERSION 2.6)

project(bal)

find_package(libv REQUIRED core lma geometry)

find_package(Boost COMPONENTS chrono system thread filesystem date_time)

#include(libv_configure_executable)

# add some linker directories to find libs if Release mode...
link_directories("${CMAKE_BINARY_DIR}/../../libv/core/Release" "${CMAKE_BINARY_DIR}/../../libv/geometry/Release")

# we bypass libv_configure_executable to not link with libv-lma
include(libv_preamble)
add_executable(${PROJECT_NAME} ${cpp_files})
target_link_libraries(${PROJECT_NAME} libv-core libv-geometry)
set(${macro_prefix}_PRIVATE_CXX_FLAGS "${libv_private_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}}" CACHE STRING "recommended compiler flags when building ${PROJECT_NAME}")
set(CMAKE_CXX_FLAGS "${LIBV_CXX_FLAGS} ${${macro_prefix}_PRIVATE_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)


target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

