set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(ASSIST_SOURCES
		AngleSolver.cpp
		AngleSolver.h
		Armor.cpp
		Armor.h
		STF.cpp
		STF.h
		ArmorDetector.cpp
		ArmorDetector.h
		)


add_library(Assist SHARED
        ${ASSIST_SOURCES})

target_link_libraries(Assist
		-lpthread
		${OpenCV_LIBS}
		CameraSet
		Protobuf
		Serial
		)


