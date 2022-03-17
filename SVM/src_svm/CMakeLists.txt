
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

add_library(src_svm  svm.cpp svm-predict.c svm-scale.c svm-train.c)

target_link_libraries(src_svm
        ${OpenCV_LIBS}
        )

