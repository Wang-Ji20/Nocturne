cmake_minimum_required(VERSION 3.11)

project(Nocturne LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build_support/cmake)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to `Debug` as none was specified.")
        set(CMAKE_BUILD_TYPE "Debug")
endif()

find_package(ALSA REQUIRED)

if (NOT ALSA_FOUND)
        message(FATAL_ERROR "ALSA not found")
endif()

message(STATUS "ALSA_LIBRARIES: ${ALSA_LIBRARIES}")

#
# !! ATTENTION
# clear cache (CMakeCache.txt) if you change this
#

find_package(FLEX REQUIRED)

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL REQUIRED)

#
# END ATTENTION
#

# use pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(NOCTURNE_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/lib/include)

include_directories(${NOCTURNE_SRC_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS})

add_subdirectory(lib)
add_subdirectory(tools)
