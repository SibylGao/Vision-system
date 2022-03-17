cmake_minimum_required(VERSION 3.12)
project(img_read)

set(CMAKE_CXX_STANDARD 14)
include_directories(/home/sibylgao/opencv-3.3.0/include)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)

add_executable(img_read main.cpp)

link_directories(/usr/local/lib)
link_directories(/home/sibylgao/opencv-3.3.0/build)
link_directories(/home/sibylgao/opencv-3.3.0/build/lib)
link_directories(/home/sibylgao/opencv-3.3.0/include)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)


find_package(OpenCV 3.3.0 REQUIRED)
set (OpenCV_DIR /home/hitcrt/CLionProjects/gao)

target_link_libraries(img_read ${OpenCV_LIBS})
