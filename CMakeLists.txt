cmake_minimum_required(VERSION 3.9)

project(SOMFYTools)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

find_package(Boost REQUIRED COMPONENTS program_options)

# https://gcc.gnu.org/ml/gcc/2017-05/msg00073.html
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -pthread ${RTLSDR_CFLAGS_OTHER}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread ${RTLSDR_LDFLAGS_OTHER}")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

add_subdirectory(librts)
add_subdirectory(src)
