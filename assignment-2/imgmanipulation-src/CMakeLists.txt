cmake_minimum_required(VERSION 3.16)
project(imgmanipulation)

find_package(OpenCV REQUIRED)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pthread")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../imgmanipulation-bin)

add_executable(img_cpy img_cpy.cpp)
target_link_libraries(img_cpy ${OpenCV_LIBS})

add_executable(img_eff img_eff.cpp)
target_link_libraries(img_eff ${OpenCV_LIBS})
