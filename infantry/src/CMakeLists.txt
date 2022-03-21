add_subdirectory(assist)
add_subdirectory(buff)
add_subdirectory(cameraset)
add_subdirectory(protobuf)
add_subdirectory(serial)

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


add_executable(infantry
        main.cpp
        Infantry.cpp
        Infantry.h)

target_link_libraries(infantry
        -lpthread
        /usr/lib/x86_64-linux-gnu/libboost_system.so
        /usr/lib/x86_64-linux-gnu/libboost_thread.so
        ${BOOST_LIBRARIES}
        ${OpenCV_LIBS}
        /usr/local/lib/libapriltag.so
        /home/sun/Software/tensorflow-master/bazel-bin/tensorflow/libtensorflow_cc.so
        Assist
        Buff
        CameraSet
        Serial
        Protobuf
        )

