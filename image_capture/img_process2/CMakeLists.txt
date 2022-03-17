cmake_minimum_required(VERSION 3.12)
project(img_process2)

set(CMAKE_CXX_STANDARD 14)
include_directories(/home/sibylgao/opencv-3.3.0/include)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
include_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)

add_executable(img_process2 main.cpp Armor.h LightBar.h Armor.cpp)

link_directories(/usr/local/lib)
link_directories(/home/sibylgao/opencv-3.3.0/build)
link_directories(/home/sibylgao/opencv-3.3.0/build/lib)
link_directories(/home/sibylgao/opencv-3.3.0/include)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv)
link_directories(/home/sibylgao/opencv-3.3.0/include/opencv2)

find_package(OpenCV 3.3.0 REQUIRED)


target_link_libraries(img_process2 ${OpenCV_LIBS})