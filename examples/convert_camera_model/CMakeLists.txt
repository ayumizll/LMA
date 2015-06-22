cmake_minimum_required(VERSION 2.6)
project(convert_camera_model)

find_package(libv REQUIRED core geometry graphic io lma)
find_package(Boost REQUIRED system chrono)
include(libv_configure_executable)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})