set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(PROTOBUF_SOURCES
		hitcrt_rm0422.pb-c.c
		hitcrt_rm0422.pb-c.h
		protobuf-c.c
		protobuf-c.h)

add_library(Protobuf SHARED
		${PROTOBUF_SOURCES}
		)