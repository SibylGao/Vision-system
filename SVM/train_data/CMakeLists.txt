

add_executable( generate_txt.cpp   read_traindata.cpp)

target_link_libraries(train_data
        ${OpenCV_LIBS}
        )

