cmake_minimum_required(VERSION 3.5)
project(infantry)

set(CMAKE_CXX_STANDARD 11)
#SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} -Wall -O3 -march=native -std=c99 -std=c++98")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pg -march=native -std=c99")
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pg -march=native")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)


find_package(OpenCV REQUIRED)
find_package(Boost  REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/src/assist
        ${PROJECT_SOURCE_DIR}/src/buff
        ${PROJECT_SOURCE_DIR}/src/cameraset
        ${PROJECT_SOURCE_DIR}/src/protobuf
        ${PROJECT_SOURCE_DIR}/src/serial
        ${OPENCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)

add_subdirectory(src)



