set(PROXY_SRC steam_proxy.cpp)

if (WIN32)
	set(PROXY_SRC ${PROXY_SRC} launcher_win.cpp plat_win.cpp proxy.rc)
else()
	set(PROXY_SRC ${PROXY_SRC} plat_unix.cpp)
endif()

# dump original gc messages, needs protobuf
set(PROXY_SRC ${PROXY_SRC} gc_dump.cpp)

set(PROTO_SRC
	${SRC_DIR}/protos/gcsdk_gcmessages.pb.cc
	${SRC_DIR}/protos/steammessages.pb.cc)

source_group("Protobufs" FILES ${PROTO_SRC})
set(PROXY_SRC ${PROXY_SRC} ${PROTO_SRC})

include_directories(${SRC_DIR}/protos)

# local protobuf for msvc
# mikkotodo source2 will need x64 binaries compiled
if(MSVC)
	include_directories(${SRC_DIR}/external/protobuf/src)

	if(CMAKE_SIZEOF_VOID_P STREQUAL "4")
		link_directories(${SRC_DIR}/external/protobuf/lib)
	elseif(CMAKE_SIZEOF_VOID_P STREQUAL "8")
		link_directories(${SRC_DIR}/external/protobuf/lib64)
	endif()

	link_libraries(
		$<$<CONFIG:Debug>:libprotobufd>
		$<$<CONFIG:RelWithDebInfo>:libprotobuf>
		$<$<CONFIG:Release>:libprotobuf>
		$<$<CONFIG:MinSizeRel>:libprotobuf>)
else()
	link_libraries(protobuf)
endif()

include_directories(${SRC_DIR}/shared ${SRC_DIR}/external/steam)
add_compile_definitions(STEAM_API_NODLL)

add_executable(gc_proxy WIN32 ${PROXY_SRC})

# for protobuf
set_property(TARGET gc_proxy PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (PROXY_NAME)
	set_target_properties(gc_proxy PROPERTIES OUTPUT_NAME ${PROXY_NAME})
endif()

if (PROXY_DIR)
	add_custom_command(TARGET gc_proxy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:gc_proxy>
		${PROXY_DIR}/$<TARGET_FILE_NAME:gc_proxy>)
endif()
