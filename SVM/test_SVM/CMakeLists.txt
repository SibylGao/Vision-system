

#add_executable(test_SVM generate_test_data.cpp)

add_library(test_SVM  generate_test_data.cpp)



target_link_libraries(test_SVM
        ${OpenCV_LIBS}
        )