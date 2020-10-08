#include "config.hpp"
#include "shape.hpp"
#include "bond.hpp"
#include "shapegroup.hpp"
#include "handwritting.hpp"
#include "mol.hpp"

#include "neural_net.h"

#include <opencv2/opencv.hpp>

#include <openbabel/mol.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

#include <torch/torch.h>

const int www = 480, hhh = 320;

void demoSingle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    auto tmp = HandWritting::GetByIntLabel(1 + rand() % 182);
    tmp.resizeTo(200, 200);
    tmp.rotate(-10 + rand() % 20);
    tmp.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    tmp.resizeTo(100, 100, false);
    tmp.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoLine() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    auto line = HandWritting::GetShape("line");
    line.castToLine(cv::Point2f(50, 50), cv::Point2f(50, 250));
    line.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoCircle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    auto circle = HandWritting::GetShape("circle");
    circle.castToCircle(cv::Point2f(400, 200), 100, 100);
    circle.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    ShapeGroup hehe(makeNotedStringByChar("1234567890"));
    hehe.resizeTo(400, 100);
    hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    hehe.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoRichShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    ShapeGroup::setSAngleK((rand() % 20 - 10) / 100.0);
    ShapeGroup::setSNormSizeK((rand() % 10 - 5) / 100.0 + 1.0);
    ShapeGroup::setSNormOffsetKx((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSNormOffsetKy((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSSubSizeK((rand() % 10 - 15) / 100.0 + 1.0);
    ShapeGroup::setSSubOffsetKx((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSSubOffsetKy((rand() % 10 - 5) / 100.0);
    ShapeGroup hehe(makeNotedString(
//                "CuSO4(5H2O)",
//                {Normal, RightButtom, Normal, Normal, RightButtom, Normal, Normal, Normal, RightButtom, Normal}
//                "SO42-",
//                {Normal, Normal, RightButtom, Up, RightTop}
            "W",
            {Normal}
//                "CuSO4",
//                {Normal, RightButtom, Normal, Normal, RightButtom}
    ));
    hehe.resizeTo(50, 50);
    hehe.moveLeftTopTo(cv::Point2f(0, 0));
//        hehe.moveCenterTo(cv::Point2f(25,25));
    hehe.rotate(180);
//        hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
//        hehe.rotate(90);
//        hehe.resizeTo(www /3, hhh /3);
    hehe.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoSpecialShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
//        auto positiveSym = ShapeGroup::GetShapeGroup("positive");
    auto positiveSym = ShapeGroup::GetShapeGroup("negative");
    positiveSym->resizeTo(200, 200);
    positiveSym->moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    positiveSym->paintTo(img1);
//        cv::imwrite("D:/tmp.jpg",img1);
//        break;
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoMol() {
    Mol::LoopOn(get_coordinates_json_path(0), "testDraw");
}

void demoBond() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
    auto bond = Bond::GetBond("DashWedge");
    bond->setUseHandWrittenWChar(true);
    bond->setVertices({cv::Point2f(20, 20),
                       cv::Point2f(www * 0.6, hhh * 0.6)});
    bond->paintTo(img1);
//        cv::imwrite("D:/tmp.jpg", img1);
//        break;
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void makeIcon() {
    const std::string dir = "C:\\Users\\xgd\\source\\COCR\\android\\app\\src\\main\\res\\drawable\\";
    std::vector<std::string> sym = {
            "positive", "negative"};
//    for (auto &s:sym) {
//        cv::Mat img = cv::Mat(256, 256, CV_8UC3, WHITE);
//        auto tmp = ShapeGroup::GetShapeGroup(s);
//        tmp->resizeTo(200, 200);
//        tmp->moveCenterTo(cv::Point2f(128,128));
//        tmp->paintTo(img);
//        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
//    }
    sym = {"SolidWedge", "DashWedge", "Single", "Double", "Triple", "Wave"};
    for (auto &s:sym) {
        cv::Mat img = cv::Mat(256, 256, CV_8UC3, WHITE);
        auto tmp = Bond::GetBond(s);
        tmp->setUseHandWrittenWChar(true);
//        tmp->resizeTo(200, 200);
        tmp->setVertices({cv::Point2f(20, 20), cv::Point2f(196, 196)});
//        tmp->moveCenterTo(cv::Point2f(128,128));
        tmp->paintTo(img);
        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
    }
}

void demo_torch() {
    std::cout << "FeedForward Neural Network\n\n";
    // Device
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << (cuda_available ? "CUDA available. Training on GPU." : "Training on CPU.") << '\n';
    // Hyper parameters
    const int64_t input_size = 784;
    const int64_t hidden_size = 500;
    const int64_t num_classes = 10;
    const int64_t batch_size = 100;
    const size_t num_epochs = 5;
    const double learning_rate = 0.001;
    const std::string MNIST_data_path = "C:/Users/xgd/.keras/datasets/pytorch-cpp/mnist/";
    // MNIST Dataset
    auto train_dataset = torch::data::datasets::MNIST(MNIST_data_path)
            .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
            .map(torch::data::transforms::Stack<>());
    // Number of samples in the training set
    auto num_train_samples = train_dataset.size().value();
    auto test_dataset = torch::data::datasets::MNIST(MNIST_data_path, torch::data::datasets::MNIST::Mode::kTest)
            .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
            .map(torch::data::transforms::Stack<>());
    // Number of samples in the testset
    auto num_test_samples = test_dataset.size().value();
    // Data loaders
    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(train_dataset), batch_size);
    auto test_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(test_dataset), batch_size);
    // Neural Network model
    NeuralNet model(input_size, hidden_size, num_classes);
    model->to(device);
    // Optimizer
    torch::optim::SGD optimizer(model->parameters(), torch::optim::SGDOptions(learning_rate));
    // Set floating point output precision
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Training...\n";
    // Train the model
    for (size_t epoch = 0; epoch != num_epochs; ++epoch) {
        // Initialize running metrics
        double running_loss = 0.0;
        size_t num_correct = 0;
        for (auto &batch : *train_loader) {
            auto data = batch.data.view({batch_size, -1}).to(device);
            auto target = batch.target.to(device);
            // Forward pass
            auto output = model->forward(data);
            auto loss = torch::nn::functional::cross_entropy(output, target);
            // Update running loss
            running_loss += loss.item<double>() * data.size(0);
            // Calculate prediction
            auto prediction = output.argmax(1);
            // Update number of correctly classified samples
            num_correct += prediction.eq(target).sum().item<int64_t>();
            // Backward and optimize
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }
        auto sample_mean_loss = running_loss / num_train_samples;
        auto accuracy = static_cast<double>(num_correct) / num_train_samples;
        std::cout << "Epoch [" << (epoch + 1) << "/" << num_epochs << "], Trainset - Loss: "
                  << sample_mean_loss << ", Accuracy: " << accuracy << '\n';
    }
    std::cout << "Training finished!\n\n";
    std::cout << "Testing...\n";
    // Test the model
    model->eval();
    torch::NoGradGuard no_grad;
    double running_loss = 0.0;
    size_t num_correct = 0;
    for (const auto &batch : *test_loader) {
        auto data = batch.data.view({batch_size, -1}).to(device);
        auto target = batch.target.to(device);
        auto output = model->forward(data);
        auto loss = torch::nn::functional::cross_entropy(output, target);
        running_loss += loss.item<double>() * data.size(0);
        auto prediction = output.argmax(1);
        num_correct += prediction.eq(target).sum().item<int64_t>();
    }
    std::cout << "Testing finished!\n";
    auto test_accuracy = static_cast<double>(num_correct) / num_test_samples;
    auto test_sample_mean_loss = running_loss / num_test_samples;
    std::cout << "Testset - Loss: " << test_sample_mean_loss << ", Accuracy: " << test_accuracy << '\n';
}

void demo_opencv() {
    auto img = cv::imread("C:/Users/xgd/Pictures/workspace/xiaoqingwa.png");
    cv::imshow("demo", img);
    cv::waitKey(0);
}

#define RET_EMPTY_STRING_IF_NOT(condition) if(!(condition))return ""

std::string demo_openbabel(const char *smi) {
    using namespace OpenBabel;
    using namespace std;
    OBConversion conv;
    try {
        auto format_in = conv.FindFormat("smiles");
        auto format_out = conv.FindFormat("pdb");
        stringstream ism(smi);
        OBMol mol;
        RET_EMPTY_STRING_IF_NOT(conv.SetInFormat(format_in));
        RET_EMPTY_STRING_IF_NOT(conv.SetOutFormat(format_out));
        RET_EMPTY_STRING_IF_NOT(conv.Read(&mol, &ism));
        auto pFF = OBForceField::FindForceField("UFF");
        RET_EMPTY_STRING_IF_NOT(pFF);
        OBBuilder builder;
        RET_EMPTY_STRING_IF_NOT(builder.Build(mol));
        RET_EMPTY_STRING_IF_NOT(mol.AddHydrogens());
        // FIXME: ret value in OBForceField::Setup is false?
        pFF->Setup(mol);
        RET_EMPTY_STRING_IF_NOT(pFF->UpdateCoordinates(mol));
        stringstream osm;
        RET_EMPTY_STRING_IF_NOT(conv.Write(&mol, &osm));
        return std::move(osm.str());
    } catch (exception e) {
        return "";
    }
}
