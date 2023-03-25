cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

project(gc_proxy)

# should be bin dir on windows, doesn't matter on linux or macos
set(PROXY_DIR CACHE STRING "Directory to output executable to")

# only relevant on windows, on linux and macos we build libgc_proxy.so/dylib
# and load it with LD_PRELOAD/DYLD_INSERT_LIBRARIES
set(PROXY_NAME CACHE STRING "Exectable name (csgo, hl2...)")

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	
	# don't crash on machines with a lot of ram
	add_link_options(/LARGEADDRESSAWARE)
	
	# newer csgo builds use several gigabytes of stack
	add_link_options(/STACK:1572864)
endif()

add_subdirectory(proxy)
