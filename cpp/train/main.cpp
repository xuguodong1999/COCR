#include "mv3.hpp"
//#include "sosodataset.hpp"
#include "couchdataset.hpp"
#include "statistic.hpp"
#include <fstream>
#include <chrono>

std::vector<std::pair<int, int>> cfg = {
        {1, 64},
        {1, 64},
        {1, 64},
        {2, 64},
        {2, 64},
        {2, 64},
        {2, 64},
        {3, 64},
        {3, 64},
        {4, 64},
};

int train() {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << device << std::endl;

    // Hyper parameters
    const int64_t batch_size = 240;
    const size_t num_epochs = 10000;
    const double learning_rate = 0.0001;
    // number of epochs after which to decay the learning rate
    const size_t learning_rate_decay_frequency = 40;
    const double learning_rate_decay_factor = 0.999;

    // CIFAR10 custom dataset
    auto train_dataset = CouchDataSet()
            .map(torch::data::transforms::Stack<>());
    int64_t num_classes = CouchDataSet::sNumOfClass;
    // Number of samples in the training set
    auto num_train_samples = train_dataset.size().value();
    // Number of samples in the testset
    auto num_test_samples = num_train_samples;//test_dataset.size().value();

    // Data loader
    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(train_dataset), batch_size);

    auto &test_loader = train_loader;

    // Model
    auto model = std::make_shared<Mv3Large>(num_classes);
    model->to(device);
    torch::load(model, "D:/mv3-epoch-6.pth");
    // Optimizer
    torch::optim::Adam optimizer(
            model->parameters(), torch::optim::AdamOptions(learning_rate));

    // Set floating point output precision
    std::cout << std::fixed << std::setprecision(4);

    auto current_learning_rate = learning_rate;

    std::cout << "Training...\n";

    // Train the model
    for (size_t epoch = 0; epoch != num_epochs; ++epoch) {
//        break;
        // Initialize running metrics
        CouchDataSet::shuffle();
        double running_loss = 0.0;
        size_t num_correct = 0;
        int batch_num = 0;
        // TODO: log each epoch
        for (auto &batch : *train_loader) {
            size_t cfg_index = rand() % cfg.size();
            RC::shapeAttr.thickness = cfg[cfg_index].first;
            CouchDataSet::setTrainSize(cfg[cfg_index].second, cfg[cfg_index].second);
            if (++batch_num == 100) {
                break;
            }
            // Transfer images and target labels to device
            auto data = batch.data.to(device);
            auto target = batch.target.to(device);
            // Forward pass
            auto output = model->forward(data);
//            std::cout << data.sizes() << std::endl;
//            std::cout << target.sizes() << std::endl;
//            std::cout << output.sizes() << std::endl;
//            target.print();
//            output.print();
            // Calculate loss
            auto loss = torch::nn::functional::cross_entropy(output, target);
            // Update running loss
            running_loss += loss.item<double>() * data.size(0);
            // Calculate prediction
            auto prediction = output.argmax(1);
//            std::cout<<prediction.sizes()<<std::endl;
            // Update number of correctly classified samples
            num_correct += prediction.eq(target).sum().item<int64_t>();
            // Backward pass and optimize
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
//            system("pause");
        }

        // Decay learning rate
        if ((epoch + 1) % learning_rate_decay_frequency == 0) {
            current_learning_rate *= learning_rate_decay_factor;
            static_cast<torch::optim::AdamOptions &>(optimizer.param_groups().front()
                    .options()).lr(current_learning_rate);
        }
        batch_num *= batch_size;
        auto sample_mean_loss = running_loss / batch_num;//num_train_samples;
        auto accuracy = static_cast<double>(num_correct) / batch_num;//num_train_samples;

        std::cout << "Epoch [" << (epoch + 1) << "/" << num_epochs << "], Trainset - Loss: "
                  << sample_mean_loss << ", Accuracy: " << accuracy << '\n';
        std::string model_save_path = "D:/mv3-epoch-" + std::to_string(epoch % 10) + ".pth";
        torch::save(model, model_save_path);
    }

    std::cout << "Training finished!\n\n";
    std::cout << "Testing...\n";

    // Test the model
    model->eval();
    torch::NoGradGuard no_grad;

    double running_loss = 0.0;
    size_t num_correct = 0;

    for (const auto &batch : *test_loader) {
        auto data = batch.data.to(device);
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


    return 0;
}

#include <opencv2/opencv.hpp>

extern std::map<int, std::wstring> gbTable;
std::vector<std::string> test_img_path = {
        "C:/Users/xgd/Pictures/yue.png",
        "C:/Users/xgd/Pictures/workspace/liang.png",
        "C:/Users/xgd/Pictures/workspace/xin.png"
};

torch::Tensor getOneSample(const std::string &_img_path = "C:/Users/xgd/Pictures/yue.png") {
    auto cvImg = cv::imread(_img_path);
    cvImg.convertTo(cvImg, CV_32F, 1.0 / 255);
//    cv::imshow("",cvImg);
//    cv::waitKey(0);
    std::cout << cvImg.size << std::endl;
    auto imgTensor = torch::from_blob(
            cvImg.data, {cvImg.cols, cvImg.rows, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous().unsqueeze(0);
    return std::move(imgTensor);
}

void test() {
//    CouchDataSet();
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << device << std::endl;
    auto model = std::make_shared<Mv3Small>(6946);
    model->to(device);
    torch::load(model, "D:/mv3-epoch-6.pth");
    model->eval();
    torch::NoGradGuard no_grad;
    auto imgTensor = getOneSample();
    imgTensor.print();
    auto data = imgTensor.to(device);
    auto output = model->forward(data);
//    std::cout<<output<<std::endl;
    int topk = 5;
    auto prediction = output.topk(topk, 1);
    std::cout << std::get<0>(prediction) << std::endl;
    auto indices = std::get<1>(prediction).squeeze(0);
    for (int i = 0; i < topk; i++) {
        std::cout << indices[i].item().toInt() << std::endl;
        std::wcout << gbTable[indices[i].item().toInt()] << std::endl;
    }
}

#include <QApplication>

void test_mv3_large() {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << device << std::endl;
    auto model = std::make_shared<Mv3Large>(62);
    torch::load(model, "D:/mv3-epoch-6.pth");
    model->to(device);
    model->eval();
    while (model.get()) {
        Timer timer;
        timer.start();
        auto input = getOneSample(test_img_path[rand() % test_img_path.size()]).to(device);
        auto output = model->forward(input);
        int topk = 5;
        auto prediction = output.topk(topk, 1);
        std::cout << std::get<0>(prediction) << std::endl;
        auto indices = std::get<1>(prediction).squeeze(0);
        for (int i = 0; i < topk; i++) {
            std::cout << indices[i].item().toInt() << std::endl;
            std::wcout << gbTable[indices[i].item().toInt()] << std::endl;
        }
        timer.stop();
        output.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main(int argc, char **argv) {
    std::wcout.imbue(std::locale("chs"));
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    try {
//        train();
//        test();
        test_mv3_large();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(0);
    return app.exec();
}

void test_couch() {
    CouchLoader::LoadCouchDataSet();
    CouchDataSet couchDataSet;
    couchDataSet.setTrainSize(216, 216);
    for (size_t i = 0; i < couchDataSet.size(); i++) {
        auto tmp = couchDataSet.get(i);
        std::cout << tmp.data.sizes() << std::endl;
        std::cout << tmp.target << std::endl;
    }
}

//void test_soso(){
//    SosoDataSet sosoDataSet("C:\\Users\\xgd\\.keras\\datasets\\SOSO-part-2020-01-26\\train.list",
//                            "C:\\Users\\xgd\\.keras\\datasets\\SOSO-part-2020-01-26\\test.list");
//    for (size_t i = 0; i < sosoDataSet.size(); i++) {
//        sosoDataSet.get(i);
//    }
//}
//void test_mv3(){
//    auto cuda_available = torch::cuda::is_available();
//    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
//    auto net = std::make_shared<MobileNetV3>(4096);
//    net->to(device);
//    auto test_img = torch::rand({10, 3, 416, 416}).to(device);
//    while (1) {
////        std::cout << test_img.sizes() << std::endl;
//        auto test_out = net->forward(test_img);
////        std::cout << test_out.sizes() << std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//
//    }
//}
//void test_load_weights() {
//    //    s model_path = "D:/mv3.pt";
//    // way 1 2 work between cpp but error from cpp to python
//    // way 1
////    torch::serialize::OutputArchive output_archive;
////    net->save(output_archive);
////    output_archive.save_to(model_path);
//    // way 2
////    torch::save(net, model_path);
//
//    // way 3
////    std::ofstream ofsm(model_path,std::ios::binary);
////    for(auto&layer:net->named_parameters()){
////        std::cout<<layer.key()<<std::endl;
////        auto dat=torch::pickle_save(layer.value());
////        ofsm.write(dat.data(),dat.size());
////    }
////    ofsm.close();
//}