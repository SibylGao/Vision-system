# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/sibylgao/clion-2018.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/sibylgao/clion-2018.2.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sibylgao/CLionProjects/SVM

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sibylgao/CLionProjects/SVM/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test_SVM.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_SVM.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_SVM.dir/flags.make

CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o: CMakeFiles/test_SVM.dir/flags.make
CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o: ../test_SVM/generate_test_data.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sibylgao/CLionProjects/SVM/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o -c /home/sibylgao/CLionProjects/SVM/test_SVM/generate_test_data.cpp

CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sibylgao/CLionProjects/SVM/test_SVM/generate_test_data.cpp > CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.i

CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sibylgao/CLionProjects/SVM/test_SVM/generate_test_data.cpp -o CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.s

# Object files for target test_SVM
test_SVM_OBJECTS = \
"CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o"

# External object files for target test_SVM
test_SVM_EXTERNAL_OBJECTS =

test_SVM: CMakeFiles/test_SVM.dir/test_SVM/generate_test_data.cpp.o
test_SVM: CMakeFiles/test_SVM.dir/build.make
test_SVM: /usr/local/lib/libopencv_dnn.so.3.3.0
test_SVM: /usr/local/lib/libopencv_ml.so.3.3.0
test_SVM: /usr/local/lib/libopencv_objdetect.so.3.3.0
test_SVM: /usr/local/lib/libopencv_shape.so.3.3.0
test_SVM: /usr/local/lib/libopencv_stitching.so.3.3.0
test_SVM: /usr/local/lib/libopencv_superres.so.3.3.0
test_SVM: /usr/local/lib/libopencv_videostab.so.3.3.0
test_SVM: /usr/local/lib/libopencv_viz.so.3.3.0
test_SVM: /usr/local/lib/libopencv_calib3d.so.3.3.0
test_SVM: /usr/local/lib/libopencv_features2d.so.3.3.0
test_SVM: /usr/local/lib/libopencv_flann.so.3.3.0
test_SVM: /usr/local/lib/libopencv_highgui.so.3.3.0
test_SVM: /usr/local/lib/libopencv_photo.so.3.3.0
test_SVM: /usr/local/lib/libopencv_video.so.3.3.0
test_SVM: /usr/local/lib/libopencv_videoio.so.3.3.0
test_SVM: /usr/local/lib/libopencv_imgcodecs.so.3.3.0
test_SVM: /usr/local/lib/libopencv_imgproc.so.3.3.0
test_SVM: /usr/local/lib/libopencv_core.so.3.3.0
test_SVM: CMakeFiles/test_SVM.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sibylgao/CLionProjects/SVM/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_SVM"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_SVM.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_SVM.dir/build: test_SVM

.PHONY : CMakeFiles/test_SVM.dir/build

CMakeFiles/test_SVM.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_SVM.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_SVM.dir/clean

CMakeFiles/test_SVM.dir/depend:
	cd /home/sibylgao/CLionProjects/SVM/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sibylgao/CLionProjects/SVM /home/sibylgao/CLionProjects/SVM /home/sibylgao/CLionProjects/SVM/cmake-build-debug /home/sibylgao/CLionProjects/SVM/cmake-build-debug /home/sibylgao/CLionProjects/SVM/cmake-build-debug/CMakeFiles/test_SVM.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_SVM.dir/depend

