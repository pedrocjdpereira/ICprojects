cmake_minimum_required(VERSION 3.16)
project(bitstream)

SET (CMAKE_BUILD_TYPE "Release")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../audiocodec-bin)

add_executable (audiocodec audiocodec.cpp)
target_link_libraries(audiocodec sndfile fftw3)
