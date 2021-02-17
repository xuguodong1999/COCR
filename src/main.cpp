//#include <QApplication>
//#include <QWidget>
//
//int main(int argc, char *argv[]) {
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
//    QApplication app(argc, argv);
//    (new QWidget)->show();
//    return app.exec();
//}

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>

std::string ROOT_DIR = "/home/xgd/source/repos/jokejoker/workspace/";
std::string std_alphabet = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";

void test_crnn() {
//    while (true) {
    int rgb = cv::IMREAD_GRAYSCALE;
    cv::Mat image = cv::imread(
            ROOT_DIR + "demo.jpg",
//            "/home/xgd/source/crnn.pytorch/data/demo2.png",
            rgb);
    cv::hconcat(image,image,image);
    cv::hconcat(image,image,image);
    cv::hconcat(image,image,image);
    cv::hconcat(image,image,image);
    /**
        layer_type=Conv
        node_proto[0].find(input.1).second=[ 1 1 32 192 ]
        layer_type=Relu
        node_proto[0].find(45).second=[ 1 32 32 192 ]
        layer_type=MaxPool
        node_proto[0].find(46).second=[ 1 32 32 192 ]
        layer_type=Conv
        node_proto[0].find(47).second=[ 1 32 16 96 ]
        layer_type=Relu
        node_proto[0].find(48).second=[ 1 64 16 96 ]
        layer_type=MaxPool
        node_proto[0].find(49).second=[ 1 64 16 96 ]
        layer_type=Conv
        node_proto[0].find(50).second=[ 1 64 8 48 ]
        layer_type=Relu
        node_proto[0].find(51).second=[ 1 128 8 48 ]
        layer_type=Conv
        node_proto[0].find(52).second=[ 1 128 8 48 ]
        layer_type=Relu
        node_proto[0].find(53).second=[ 1 128 8 48 ]
        layer_type=MaxPool
        node_proto[0].find(54).second=[ 1 128 8 48 ]
        layer_type=Conv
        node_proto[0].find(55).second=[ 1 128 4 48 ]
        layer_type=Relu
        node_proto[0].find(202).second=[ 1 256 4 48 ]
        layer_type=Conv
        node_proto[0].find(58).second=[ 1 256 4 48 ]
        layer_type=Relu
        node_proto[0].find(205).second=[ 1 256 4 48 ]
        layer_type=MaxPool
        node_proto[0].find(61).second=[ 1 256 4 48 ]
        layer_type=Conv
        node_proto[0].find(62).second=[ 1 256 2 48 ]
        layer_type=Relu
        node_proto[0].find(63).second=[ 1 256 1 47 ]
        layer_type=Transpose
        node_proto[0].find(64).second=[ 1 256 1 47 ]
        _numAxes=4
        inputs[0].size()=4
        layer_type=AveragePool
        node_proto[0].find(65).second=[ 1 47 256 1 ]
        layer_type=Squeeze
        inpShape=[ 1 47 256 1 ]
        outShape=[ 1 47 256 ]
        layerParams.type=Reshape
        hasDynamicShapes=0
        node_proto[0].find(66).second=[ 1 47 256 1 ]
        layer_type=Shape
        layer_type=Constant
        layer_type=Gather
        layer_type=Unsqueeze
        layer_type=Concat
        layer_type=ConstantOfShape
        layer_type=Transpose
        node_proto[0].find(67).second=[ 1 47 256 ]
        _numAxes=3
        inputs[0].size()=3
        layer_type=LSTM
        node_proto[0].find(78).second=[ 47 1 256 ]
        node_proto[0].find(123/lstm).second=[ 47 1 512 ]
        layer_type=Transpose
        node_proto[0].find(123).second=[ 47 1 1 512 ]
        _numAxes=4
        inputs[0].size()=4
        layer_type=Constant
        layer_type=Reshape
        node_proto[0].find(126).second=[ 47 1 1 512 ]
        layer_type=Transpose
        node_proto[0].find(128).second=[ 47 1 512 ]
        _numAxes=3
        inputs[0].size()=3
        layer_type=MatMul
        node_proto[0].find(129).second=[ 1 47 512 ]
        layer_type=Add
        node_proto[0].find(131).second=[ 1 47 256 ]
        layer_type=Shape
        layer_type=Constant
        layer_type=Gather
        layer_type=Unsqueeze
        layer_type=Concat
        layer_type=ConstantOfShape
        layer_type=Transpose
        node_proto[0].find(132).second=[ 1 47 256 ]
        _numAxes=3
        inputs[0].size()=3
        layer_type=LSTM
        node_proto[0].find(143).second=[ 47 1 256 ]
        node_proto[0].find(188/lstm).second=[ 47 1 512 ]
        layer_type=Transpose
        node_proto[0].find(188).second=[ 47 1 1 512 ]
        _numAxes=4
        inputs[0].size()=4
        layer_type=Constant
        layer_type=Reshape
        node_proto[0].find(191).second=[ 47 1 1 512 ]
        layer_type=Transpose
        node_proto[0].find(193).second=[ 47 1 512 ]
        _numAxes=3
        inputs[0].size()=3
        layer_type=MatMul
        node_proto[0].find(194).second=[ 1 47 512 ]
        layer_type=Add
        node_proto[0].find(196).second=[ 1 47 256 ]
        layer_type=MatMul
        node_proto[0].find(197).second=[ 1 47 256 ]
        layer_type=Add
        node_proto[0].find(199).second=[ 1 47 58 ]
        layer_type=Transpose
        node_proto[0].find(200).second=[ 1 47 58 ]
        _numAxes=3
        inputs[0].size()=3
        _numAxes=4
        inputs[0].size()=4
        _numAxes=3
        inputs[0].size()=3
        _numAxes=4
        inputs[0].size()=4
        _numAxes=3
        inputs[0].size()=3
        _numAxes=3
        inputs[0].size()=3
        _numAxes=4
        inputs[0].size()=4
        _numAxes=3
        inputs[0].size()=3
        _numAxes=3
        inputs[0].size()=3
     */
    cv::dnn::TextRecognitionModel model(
            ROOT_DIR + "crnn_sim_3200.onnx"//1x47x57 47-192
//            ROOT_DIR + "crnn.onnx"//1x47x57 47-192
    );
    auto &net = model.getNetwork_();
//    for (auto &l:net.getLayerNames()) {
//        std::cout << l << std::endl;
//    }
    model.setDecodeType("CTC-greedy");
    std::vector<std::string> vocabulary;
    for (auto &s:std_alphabet) {
        vocabulary.push_back(std::string(1, s));
    }

    model.setVocabulary(vocabulary);
    auto vb = model.getVocabulary();
//    for (auto &v:vb) {
//        std::cout << v << std::endl;
//    }
    double scale = 1.0 / 127.5;
    cv::Scalar mean = cv::Scalar(127.5, 127.5, 127.5);

    cv::Size inputSize = cv::Size(3200, 32);
    std::cerr << "here1" << std::endl;
    model.setInputParams(scale, inputSize, mean);
    std::cerr << "here2" << std::endl;
    std::string recognitionResult = model.recognize(image);
    std::cout << "'" << recognitionResult << "'" << std::endl;
    cv::imshow("result=" + recognitionResult, image);
    cv::waitKey(0);
//    }
}

int main(int argc, char **args) {
    try {
        test_crnn();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
