/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// A minimal but useful C++ example showing how to load an Imagenet-style object
// recognition TensorFlow model, prepare input images for it, run them through
// the graph, and interpret the results.
//
// It's designed to have as few dependencies and be as clear as possible, so
// it's more verbose than it could be in production code. In particular, using
// auto for the types of a lot of the returned values from TensorFlow calls can
// remove a lot of boilerplate, but I find the explicit types useful in sample
// code to make it simple to look up the classes involved.
//
// To use it, compile and then run in a working directory with the
// learning/brain/tutorials/label_image/data/ folder below it, and you should
// see the top five labels for the example Lena image output. You can then
// customize it to use your own models or images by changing the file names at
// the top of the main() function.
//
// The googlenet_graph.pb file included by default is created from Inception.
//
// Note that, for GIF inputs, to reuse existing code, only single-frame ones
// are supported.

#include "label_image.h"
// Takes a file name, and loads a list of labels from it, one per line, and
// returns a vector of the strings. It pads with empty strings so the length
// of the result is a multiple of 16, because our model expects that.
Status classifier::ReadLabelsFile(const string& file_name, std::vector<string>* result,
                      size_t* found_label_count) {
  std::ifstream file(file_name);
  if (!file) {
    return tensorflow::errors::NotFound("Labels file ", file_name,
                                        " not found.");
  }
  result->clear();
  string line;
  while (std::getline(file, line)) {
    result->push_back(line);
  }
  *found_label_count = result->size();
  const int padding = 16;
  while (result->size() % padding) {
    result->emplace_back();
  }
  return Status::OK();
}

Status classifier::ReadEntireFile(tensorflow::Env* env, const string& filename,
                             Tensor* output) {
  tensorflow::uint64 file_size = 0;
  TF_RETURN_IF_ERROR(env->GetFileSize(filename, &file_size));

  string contents;
  contents.resize(file_size);

  std::unique_ptr<tensorflow::RandomAccessFile> file;
  TF_RETURN_IF_ERROR(env->NewRandomAccessFile(filename, &file));

  tensorflow::StringPiece data;
  TF_RETURN_IF_ERROR(file->Read(0, file_size, &data, &(contents)[0]));
  if (data.size() != file_size) {
    return tensorflow::errors::DataLoss("Truncated read of '", filename,
                                        "' expected ", file_size, " got ",
                                        data.size());
  }
  output->scalar<string>()() = data.ToString();
  return Status::OK();
}

// Given an image file name, read in the data, try to decode it as an image,
// resize it to the requested size, and then scale the values as desired.
Status classifier::ReadTensorFromImageFile(const string& file_name, const int input_height,
                               const int input_width, const float input_mean,
                               const float input_std,
                               std::vector<Tensor>* out_tensors) {
  auto root = tensorflow::Scope::NewRootScope();
  using namespace ::tensorflow::ops;  // NOLINT(build/namespaces)

  string input_name = "file_reader";
  string output_name = "normalized";

  // read file_name into a tensor named input
  Tensor input(tensorflow::DT_STRING, tensorflow::TensorShape());
  TF_RETURN_IF_ERROR(
      ReadEntireFile(tensorflow::Env::Default(), file_name, &input));

  // use a placeholder to read input data
  auto file_reader =
      Placeholder(root.WithOpName("input"), tensorflow::DataType::DT_STRING);

  std::vector<std::pair<string, tensorflow::Tensor>> inputs = {
      {"input", input},
  };

  // Now try to figure out what kind of file it is and decode it.
  const int wanted_channels = 1;//3
  tensorflow::Output image_reader;
  if (tensorflow::StringPiece(file_name).ends_with(".png")) {
    image_reader = DecodePng(root.WithOpName("png_reader"), file_reader,
                             DecodePng::Channels(wanted_channels));
  } else if (tensorflow::StringPiece(file_name).ends_with(".gif")) {
    // gif decoder returns 4-D tensor, remove the first dim
    image_reader =
        Squeeze(root.WithOpName("squeeze_first_dim"),
                DecodeGif(root.WithOpName("gif_reader"), file_reader));
  } else if (tensorflow::StringPiece(file_name).ends_with(".bmp")) {
    image_reader = DecodeBmp(root.WithOpName("bmp_reader"), file_reader);
  } else {
    // Assume if it's neither a PNG nor a GIF then it must be a JPEG.
    image_reader = DecodeJpeg(root.WithOpName("jpeg_reader"), file_reader,
                              DecodeJpeg::Channels(wanted_channels));
  }
  // Now cast the image data to float so we can do normal math on it.
  auto float_caster =
      Cast(root.WithOpName("float_caster"), image_reader, tensorflow::DT_FLOAT);
  // The convention for image ops in TensorFlow is that all images are expected
  // to be in batches, so that they're four-dimensional arrays with indices of
  // [batch, height, width, channel]. Because we only have a single image, we
  // have to add a batch dimension of 1 to the start with ExpandDims().
  auto dims_expander = ExpandDims(root, float_caster, 0);
  // Bilinearly resize the image to fit the required dimensions.
  auto resized = ResizeBilinear(
      root, dims_expander,
      Const(root.WithOpName("size"), {input_height, input_width}));
  // Subtract the mean and divide by the scale.
  Div(root.WithOpName(output_name), Sub(root, resized, {input_mean}),
      {input_std});

  // This runs the GraphDef network definition that we've just constructed, and
  // returns the results in the output tensor.
  tensorflow::GraphDef graph;
  TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));

  std::unique_ptr<tensorflow::Session> session(
      tensorflow::NewSession(tensorflow::SessionOptions()));
  TF_RETURN_IF_ERROR(session->Create(graph));
  TF_RETURN_IF_ERROR(session->Run({inputs}, {output_name}, {}, out_tensors));
  return Status::OK();
}

// Analyzes the output of the Inception graph to retrieve the highest scores and
// their positions in the tensor, which correspond to categories.
Status classifier::GetTopLabels(const std::vector<Tensor>& outputs, int how_many_labels,
                    Tensor* indices, Tensor* scores) {
  auto root = tensorflow::Scope::NewRootScope();
  using namespace ::tensorflow::ops;  // NOLINT(build/namespaces)

  string output_name = "top_k";
  TopK(root.WithOpName(output_name), outputs[0], how_many_labels);
  // This runs the GraphDef network definition that we've just constructed, and
  // returns the results in the output tensors.
  tensorflow::GraphDef graph;
  TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));

  std::unique_ptr<tensorflow::Session> session(
      tensorflow::NewSession(tensorflow::SessionOptions()));
  TF_RETURN_IF_ERROR(session->Create(graph));
  // The TopK node returns two outputs, the scores and their original indices,
  // so we have to append :0 and :1 to specify them both.
  std::vector<Tensor> out_tensors;
  TF_RETURN_IF_ERROR(session->Run({}, {output_name + ":0", output_name + ":1"},
                                  {}, &out_tensors));
  *scores = out_tensors[0];
  *indices = out_tensors[1];
  return Status::OK();
}

// Given the output of a model run, and the name of a file containing the labels
// this prints out the top five highest-scoring values.
Status classifier::PrintTopLabels(const std::vector<Tensor>& outputs,
                      const string& labels_file_name) {
  std::vector<string> labels;
  size_t label_count;
  Status read_labels_status =
      ReadLabelsFile(labels_file_name, &labels, &label_count);
  if (!read_labels_status.ok()) {
    LOG(ERROR) << read_labels_status;
    return read_labels_status;
  }
  const int how_many_labels = std::min(5, static_cast<int>(label_count));
  Tensor indices;
  Tensor scores;
  TF_RETURN_IF_ERROR(GetTopLabels(outputs, how_many_labels, &indices, &scores));
  tensorflow::TTypes<float>::Flat scores_flat = scores.flat<float>();
  tensorflow::TTypes<int32>::Flat indices_flat = indices.flat<int32>();
  for (int pos = 0; pos < how_many_labels; ++pos) {
    const int label_index = indices_flat(pos);
    const float score = scores_flat(pos);
    //LOG(INFO) << labels[label_index] << " (" << label_index << "): " << score;
  }
  return Status::OK();
}

// This is a testing function that returns whether the top label index is the
// one that's expected.
Status classifier::CheckTopLabel(const std::vector<Tensor>& outputs, int expected,
                     bool* is_expected) {
  *is_expected = false;
  Tensor indices;
  Tensor scores;
  const int how_many_labels = 1;
  TF_RETURN_IF_ERROR(GetTopLabels(outputs, how_many_labels, &indices, &scores));
  tensorflow::TTypes<int32>::Flat indices_flat = indices.flat<int32>();
  if (indices_flat(0) != expected) {
    LOG(ERROR) << "Expected label #" << expected << " but got #"
               << indices_flat(0);
    *is_expected = false;
  } else {
    *is_expected = true;
  }
  return Status::OK();
}
// Reads a model graph definition from disk, and creates a session object you
// can use to run it.
Status classifier::LoadGraph(const string& graph_file_name,
                             std::unique_ptr<tensorflow::Session>* session) {
  tensorflow::GraphDef graph_def;
  Status load_graph_status =
          ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
  if (!load_graph_status.ok()) {
    return tensorflow::errors::NotFound("Failed to load compute graph at '",
                                        graph_file_name, "'");
  }
  session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
  Status session_create_status = (*session)->Create(graph_def);
  if (!session_create_status.ok()) {
    return session_create_status;
  }
  return Status::OK();
}

bool classifier::label_img(cv::Mat& img, std::vector<float>& confidence) {
  bool self_test = false;
//  // We need to call this to set up global state for TensorFlow.
//  tensorflow::port::InitMain(argv[0], &argc, &argv);
//  if (argc > 1) {
//    LOG(ERROR) << "Unknown argument " << argv[1] << "\n" << usage;
//    return -1;
//  }


//  // Get the image from disk as a float array of numbers, resized and normalized
//  // to the specifications the main graph expects.
//  std::vector<Tensor> resized_tensors;
//  Status read_tensor_status =
//      ReadTensorFromImageFile(image, input_height, input_width, input_mean,
//                              input_std, &resized_tensors);
//  if (!read_tensor_status.ok()) {
//    LOG(ERROR) << read_tensor_status;
//    return -1;
//  }
//  const Tensor& resized_tensor = resized_tensors[0];

  //    ///Tensor assign demo
//    Tensor W (DT_FLOAT,TensorShape({2}));
//    Tensor x1(DT_FLOAT,TensorShape({2}));
//    auto W_map  = W .tensor<float,1>();
//    auto x1_map = x1.tensor<float,1>();
//
//    for(int i=0;i<2;++i) {
//        W_map(i)  = -1;
//        x1_map(i) =  1;
//    }
//    std::cout<<"W  \n"<<W .flat<float>()<<"\n debug "<<W .DebugString()<<std::endl;
//    std::cout<<"x1 \n"<<x1.flat<float>()<<"\n debug "<<x1.DebugString()<<std::endl;

//    cv::Mat img = cv::imread(image);
    cv::Mat resized_img;
    cv::resize(img,resized_img,cv::Size(input_width,input_height),cv::INTER_LINEAR);
    Tensor input_tensor(tensorflow::DT_FLOAT,TensorShape({1, input_width, input_height, 1}));
    auto input_tensor_map = input_tensor.tensor<float,4>();

//  Tensor input_tensor(tensorflow::DT_FLOAT,TensorShape({1, 784}));
//  auto input_tensor_map = input_tensor.tensor<float,2>();
    for(int i=0; i<input_height; i++){
        for(int j=0; j<input_width; j++){
            input_tensor_map(i*input_width+j) = resized_img.at<uchar>(i,j)/255.0;
        }
    }


//    Tensor dropout(tensorflow::DT_FLOAT, TensorShape({1}));
//    auto dropout_map = dropout.tensor<float,1>();
//    dropout_map(0) = 1.0;
//    std::cout<<"dropout \n"<<dropout.flat<float>()<<"\n debug"<<dropout.DebugString()<<std::endl;
//    std::cout<<"resized_tensor \n"<<resized_tensor.flat<float>()<<"\n debug"<<resized_tensor.DebugString()<<std::endl;
//    std::cout<<"input_tensor \n"<<input_tensor.flat<float>()<<"\n debug"<<input_tensor.DebugString()<<std::endl;
    Input::Initializer input_tensor_b(float(1.0), TensorShape({1}));
  // Actually run the image through the model.
    std::vector<Tensor> outputs;
//  Status run_status = session->Run({{input_layer, input_tensor},{"dropout/Placeholder:0",dropout}},
//                                   {output_layer}, {}, &outputs);


  Status run_status = session->Run({{input_layer, input_tensor}},
                                   {output_layer}, {}, &outputs);
  if (!run_status.ok()) {
    LOG(ERROR) << "Running model failed: " << run_status;
    return false;
  }
//    std::cout<<"outputs: "<<outputs[0].flat<float>()<< std::endl;
    auto outputs_map = outputs[0].tensor<float,2>();
    confidence.reserve(outputs_map.size());
    for(int i=0;i<outputs_map.size();i++){
        confidence[i] = outputs_map(i);
//        std::cout<<"confidence "<<confidence[i]<<std::endl;
    }

//  // This is for automated testing to make sure we get the expected result with
//  // the default settings. We know that label 653 (military uniform) should be
//  // the top label for the Admiral Hopper image.
//  if (self_test) {
//    bool expected_matches;
//    Status check_status = CheckTopLabel(outputs, 653, &expected_matches);
//    if (!check_status.ok()) {
//      LOG(ERROR) << "Running check failed: " << check_status;
//      return -1;
//    }
//    if (!expected_matches) {
//      LOG(ERROR) << "Self-test failed!";
//      return -1;
//    }
//  }

  // Do something interesting with the results we've generated.
  Status print_status = PrintTopLabels(outputs, labels_path);
  if (!print_status.ok()) {
    LOG(ERROR) << "Running print failed: " << print_status;
    return false;
  }
  return true;
}

bool classifier::label_imgs(std::vector<cv::Mat>& imgs, std::vector<std::vector<float>>& confidence, int buffFlag) {

    if(lastBuffFlag == 0 || fabs(buffFlag-lastBuffFlag) == 1){
        initFlag = false;
    }

  if(!initFlag){
    if (buffFlag == 2){
      graph_path = "/home/sun/HITCRT/Infantry/data/buff/model_lenet/small_buff.pb";
      tf_init();
      lastBuffFlag = 2;
      std::cout<<"Label: Load small buff graph" << std::endl;
    } else if(buffFlag == 3){
      graph_path = "/home/sun/HITCRT/Infantry/data/buff/model_lenet/big_buff_bw.pb";
      std::cout<<"Label: Load big buff graph" << std::endl;
      tf_init();
      lastBuffFlag = 3;
    } else {
      std::cout << "Label: buffFlag is wrong!!!" << std::endl;
    }
    initFlag = true;
  }


  bool self_test = false;
  std::vector<cv::Mat> resized_imgs;
  for(int i = 0; i<imgs.size(); i++){
    cv::Mat imgTmp;
    cv::resize(imgs[i],imgTmp,cv::Size(input_width,input_height),cv::INTER_LINEAR);
    resized_imgs.push_back(imgTmp);
//    cv::imshow("resized img",imgTmp);
  }

//    cv::cvtColor(resized_img,resized_img,cv::COLOR_BGR2GRAY);
//    resized_img = 255 - resized_img;
//    cv::threshold(resized_img,resized_img,0,255,cv::THRESH_OTSU);

  Tensor input_tensor(tensorflow::DT_FLOAT,TensorShape({imgs.size(), input_width, input_height, 1}));
  auto input_tensor_map = input_tensor.tensor<float,4>();

//  Tensor input_tensor(tensorflow::DT_FLOAT,TensorShape({1, 784}));
//  auto input_tensor_map = input_tensor.tensor<float,2>();


    for(int k = 0; k<resized_imgs.size(); k++) {
        for (int i = 0; i < input_height; i++) {
            for (int j = 0; j < input_width; j++) {
                input_tensor_map(k*(input_width * input_height) + i * input_width + j) = resized_imgs[k].at<uchar>(i, j) / 255.0;
            }
        }
    }

  /*for(int k = 0; k<resized_imgs.size(); k++) {
      for (int n = 0; n<3; n++){
          for (int i = 0; i < input_height; i++) {
              for (int j = 0; j < input_width; j++) {
                  input_tensor_map(k*(input_width * input_height)*3 +n*(input_width * input_height) + i * input_width + j) = resized_imgs[k].at<cv::Vec3b>(i, j)[n] / 255.0;
              }
          }
      }
  }*/

    ///dropout
//    Tensor dropout(tensorflow::DT_FLOAT, TensorShape({1}));
//  auto dropout_map = dropout.tensor<float,1>();
//  dropout_map(0) = 1.0;
//    std::cout<<"dropout \n"<<dropout.flat<float>()<<"\n debug"<<dropout.DebugString()<<std::endl;
//    std::cout<<"resized_tensor \n"<<resized_tensor.flat<float>()<<"\n debug"<<resized_tensor.DebugString()<<std::endl;

//    std::cout<<"input_tensor \n"<<input_tensor.flat<float>()<<"\n debug"<<input_tensor.DebugString()<<std::endl;
  Input::Initializer input_tensor_b(float(1.0), TensorShape({resized_imgs.size()}));
  // Actually run the image through the model.
  std::vector<Tensor> outputs;
  Status run_status;
    ///dropout
  if(buffFlag == 2){
    Tensor dropout(tensorflow::DT_FLOAT, TensorShape({1}));
    auto dropout_map = dropout.tensor<float,1>();
    dropout_map(0) = 1.0;
    run_status = session->Run({{input_layer, input_tensor},{"dropout/Placeholder:0",dropout}},
                                   {output_layer}, {}, &outputs);
  }
  else{
    run_status = session->Run({{input_layer, input_tensor}},
                                     {output_layer}, {}, &outputs);
  }
//

  if (!run_status.ok()) {
    LOG(ERROR) << "Running model failed: " << run_status;
    return false;
  }
//    std::cout<<"outputs: "<<outputs[0].flat<float>()<< std::endl;
////////
    auto outputs_map = outputs[0].tensor<float,2>();

//    std::cout<<"1.confidence size"<<confidence.size()<< std::endl;

  if(outputs_map.size()%9 != 0){
  std::cout << "get confidence from CNN failed" << std::endl;
  } else {
      for (int k = 0; k < confidence.size(); k++) {
          for (int i = 0; i < 10; i++) {
              confidence[k].push_back(outputs_map(10 * k + i));
          }
      }
  }
//
//  for(int k = 0; k < confidence.size(); k++) {
//    for(int i = 0; i < 10; i++){
//      confidence[k].push_back(outputs_map(10*k+i));
//    }
//  }


//  }
//    std::cout<<"2.confidence size"<<confidence.size()<< std::endl;

//    for (int i = 0; i < confidence.size();i++){
//        for(int j = 0; j < confidence[i].size(); j++){
//            std::cout<< "confidence" << j << " " << confidence[i][j] << std::endl;
//        }
//        std::cout << std::endl;
//    }




//  // This is for automated testing to make sure we get the expected result with
//  // the default settings. We know that label 653 (military uniform) should be
//  // the top label for the Admiral Hopper image.
//  if (self_test) {
//    bool expected_matches;
//    Status check_status = CheckTopLabel(outputs, 653, &expected_matches);
//    if (!check_status.ok()) {
//      LOG(ERROR) << "Running check failed: " << check_status;
//      return -1;
//    }
//    if (!expected_matches) {
//      LOG(ERROR) << "Self-test failed!";
//      return -1;
//    }
//  }

  // Do something interesting with the results we've generated.
  Status print_status = PrintTopLabels(outputs, labels_path);
  if (!print_status.ok()) {
    LOG(ERROR) << "Running print failed: " << print_status;
    return false;
  }
//    cv::waitKey(0);
  return true;
}

bool classifier::tf_init(){
  // First we load and initialize the model.
  Status load_graph_status = LoadGraph(graph_path, &session);
  if (!load_graph_status.ok()) {
    LOG(ERROR) << load_graph_status;
    return false;
  }
  //std::cout<<"tensorflow load graph succesfully "<< graph_path <<std::endl;
  return true;
}
