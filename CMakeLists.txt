cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

project(cs2_patch)

set(OUT_DIR CACHE STRING "Directory to output executable to")

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

add_subdirectory(launcher)
