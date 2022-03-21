//
// Created by kx on 17-12-12.
//

#ifndef BUFF_LABEL_IMAGE_H
#define BUFF_LABEL_IMAGE_H
#include <fstream>
#include <utility>
#include <vector>

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"

#include <opencv2/opencv.hpp>

using namespace tensorflow;
//using namespace tensorflow::ops;
// These are all common classes it's handy to reference with no namespace.
using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::int32;

class classifier{

private:
    std::unique_ptr<tensorflow::Session> session;

    Status ReadLabelsFile(const string& file_name, std::vector<string>* result,
                          size_t* found_label_count);
    Status ReadEntireFile(tensorflow::Env* env, const string& filename,
                                 Tensor* output);
    Status ReadTensorFromImageFile(const string& file_name, const int input_height,
                                   const int input_width, const float input_mean,
                                   const float input_std,
                                   std::vector<Tensor>* out_tensors);
    Status LoadGraph(const string& graph_file_name,
                     std::unique_ptr<tensorflow::Session>* session);
    Status GetTopLabels(const std::vector<Tensor>& outputs, int how_many_labels,
                        Tensor* indices, Tensor* scores);
    Status PrintTopLabels(const std::vector<Tensor>& outputs,
                          const string& labels_file_name);
    Status CheckTopLabel(const std::vector<Tensor>& outputs, int expected,
                         bool* is_expected);

public:
    string graph_path;
    string labels_path = "/home/sun/HITCRT/Infantry/data/buff/model_lenet/mnist_labels.txt";
    int32 input_width = 28;//28
    int32 input_height = 28;//28
    string input_layer = "input:0";// input
    string output_layer = "prediction/out:0";//output

    bool initFlag = false;
    int lastBuffFlag = 0;

    bool tf_init();
    bool label_img(cv::Mat& img,std::vector<float>& confidence);
    bool label_imgs(std::vector<cv::Mat>& imgs, std::vector<std::vector<float>>& confidence, int buffFlag);
};


#endif //BUFF_LABEL_IMAGE_H
