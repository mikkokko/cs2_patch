set(LAUNCHER_SRC steam_proxy.cpp)

if (WIN32)
	set(LAUNCHER_SRC ${LAUNCHER_SRC} launcher_win.cpp plat_win.cpp launcher.rc)
else()
	set(LAUNCHER_SRC ${LAUNCHER_SRC} plat_unix.cpp)
endif()

set(PROTO_SRC
	${SRC_DIR}/protos/gcsdk_gcmessages.pb.cc
	${SRC_DIR}/protos/steammessages.pb.cc)

source_group("Protobufs" FILES ${PROTO_SRC})
set(LAUNCHER_SRC ${LAUNCHER_SRC} ${PROTO_SRC})

include_directories(${SRC_DIR}/protos)

# local protobuf for msvc
if(MSVC)
	include_directories(${SRC_DIR}/external/protobuf/src)
	link_directories(${SRC_DIR}/external/protobuf/lib64)

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

add_executable(cs2 WIN32 ${LAUNCHER_SRC})

# for protobuf
set_property(TARGET cs2 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (OUT_DIR)
	add_custom_command(TARGET cs2 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:cs2>
		${OUT_DIR}/$<TARGET_FILE_NAME:cs2>)
endif()
