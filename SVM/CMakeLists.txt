cmake_minimum_required(VERSION 3.12)
project(SVM)

set(CMAKE_CXX_STANDARD 14)

include_directories(/home/sibylgao/opencv-3.3.0/include)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)

link_directories(/usr/local/lib)
link_directories(/home/sibylgao/opencv-3.3.0/build)
link_directories(/home/sibylgao/opencv-3.3.0/build/lib)
link_directories(/home/sibylgao/opencv-3.3.0/include)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)

find_package(OpenCV 3.3.0 REQUIRED)


add_executable(SVM main.cpp )
add_executable( train_data train_data/read_traindata.cpp )
#add_executable(train_data2 train_data/generate_txt.cpp)
add_executable(test_SVM test_SVM/generate_test_data.cpp )


target_link_libraries(SVM
        ${OpenCV_LIBS}
        src_svm
        )

target_link_libraries(test_SVM
        ${OpenCV_LIBS}
        )

target_link_libraries(train_data
        ${OpenCV_LIBS}
        )

#target_link_libraries(train_data
#        ${OpenCV_LIBS}
#        )

#add_subdirectory(test_SVM)
add_subdirectory(src_svm)

