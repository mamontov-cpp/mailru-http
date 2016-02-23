# CMake приведен только для Linux - под виндой есть решение под MSVC, а делать поддержку MinGW пока некогда
cmake_minimum_required(VERSION 2.8)
project(final)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake")
set(LIBUV_USE_STATIC ON)

find_package(LibUV REQUIRED)

set(SRCS "main.cpp" "cmd/parse.cpp" "cmd/daemonize.cpp" "sys/thread.cpp" "sys/mutex.cpp" "sys/log.cpp" "server/on_connect.cpp")
set(HDRS "config.h" "cmd/parse.h" "cmd/daemonize.h" "sys/thread.h" "sys/mutex.h" "sys/log.h" "server/on_connect.h")

IF (WIN32)
  add_definitions(-DWIN32)
  LIST(APPEND HDRS "getopt/getopt.h")
  LIST(APPEND SRCS "getopt/getopt.c")  
ELSE()
  add_definitions(-DLINUX)
  set(GLOBAL_LIBS m rt pthread)
ENDIF()

include_directories(${LIBUV_INCLUDE_DIRS})

add_executable("final"  ${SRCS} ${HDRS})

target_link_libraries("final" ${GLOBAL_LIBS})
target_link_libraries("final" ${LIBUV_LIBRARIES})
