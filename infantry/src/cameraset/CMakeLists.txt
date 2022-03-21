set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CAMERASET_SOURCES
		myVideoCap.cpp
		myVideoCap.h
		v4l2uvc.h
		v4l2uvc.c)

add_library(CameraSet SHARED
		${CAMERASET_SOURCES})

target_link_libraries(CameraSet
		${OpenCV_LIBS})

add_executable(camera_test main.cpp
		)

target_link_libraries(camera_test
		-lpthread
		${Boost_LIBRARIES}
		${OpenCV_LIBS}
		CameraSet
		)