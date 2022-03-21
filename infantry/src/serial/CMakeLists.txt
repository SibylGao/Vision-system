set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(SERIAL_SOURCES
		serialapp.cpp
		serialapp.h
		serialbase.cpp
		serialbase.h)
add_library(Serial SHARED
		${SERIAL_SOURCES}
        )
