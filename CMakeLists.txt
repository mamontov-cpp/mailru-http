# CMake приведен только для Linux - под виндой есть решение под MSVC, а делать поддержку MinGW пока некогда
cmake_minimum_required(VERSION 2.8)
project(final)

set(SRCS "main.cpp" "cmd/parse.cpp" "cmd/daemonize.cpp")
set(HDRS "cmd/parse.h")

IF (WIN32)
  add_definitions(-DWIN32)
  LIST(APPEND HDRS "getopt/getopt.h")
  LIST(APPEND SRCS "getopt/getopt.c")  
ELSE()
  add_definitions(-DLINUX)
  set(GLOBAL_LIBS m rt pthread)
ENDIF()

add_executable("final"  ${SRCS} ${HDRS})

target_link_libraries("final" ${GLOBAL_LIBS})
