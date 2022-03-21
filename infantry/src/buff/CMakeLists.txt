set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(BUFF_SOURCES
		BuffHiter.cpp
		BuffHiter.h
		BuffParam.cpp
		BuffParam.h
		NumDecoder.h
		NumDecoder.cpp
		AngleCal.cpp
		AngleCal.h
		AprilTag.h
		AprilTag.c
		label_image.h
		label_image.cc
		NumDecoder.h
		NumDecoder.cpp
		QuadExtractor.h
		QuadExtractor.cpp
		Quad.h
		Quad.cpp
		)

include_directories(
		/usr/local/include/apriltag
		/home/sun/Software/tensorflow-master
		/home/sun/Software/tensorflow-master/tensorflow/bazel-genfiles
		/home/sun/Software/tensorflow-master/bazel-genfiles
		/home/sun/Software/tensorflow-master/tensorflow/contrib/makefile/downloads/protobuf/src
		/home/sun/Software/tensorflow-master/tensorflow/contrib/makefile/downloads/nsync/public
		/home/sun/Software/tensorflow-master/tensorflow/contrib/makefile/downloads/eigen
		/home/sun/Software/tensorflow-master/third_party/eigen3
)

add_library(Buff SHARED
        ${BUFF_SOURCES})

target_link_libraries(Buff
        ${OpenCV_LIBS}
		-lpthread
		/usr/lib/x86_64-linux-gnu/libboost_system.so
		/usr/lib/x86_64-linux-gnu/libboost_thread.so
		${BOOST_LIBRARIES}
		${OpenCV_LIBS}
        /usr/local/lib/libapriltag.so
        /home/sun/Software/tensorflow-master/bazel-bin/tensorflow/libtensorflow_cc.so
		CameraSet
		Protobuf
		)

add_executable(buff hitBuff.cpp)

target_link_libraries(buff
		-lpthread
		/usr/lib/x86_64-linux-gnu/libboost_system.so
		/usr/lib/x86_64-linux-gnu/libboost_thread.so
		${BOOST_LIBRARIES}
		${OpenCV_LIBS}
		Buff
		Serial
		CameraSet
		Protobuf
		)



add_executable(buff_test hitBuffWithVideo.cpp)
target_link_libraries(buff_test
		-lpthread
		/usr/lib/x86_64-linux-gnu/libboost_system.so
		/usr/lib/x86_64-linux-gnu/libboost_thread.so
		${BOOST_LIBRARIES}
		${OpenCV_LIBS}
		Buff
		Serial
		CameraSet
		Protobuf
		)

