#include "mv3.hpp"

//#include "sosodataset.hpp"
#include "couchdataset.hpp"
#include "statistic.hpp"
#include <fstream>
#include <chrono>
#include <opencv2/imgcodecs.hpp>

//extern std::unordered_map<int, std::wstring> gbTable;
std::unordered_map<int, std::wstring> gbTable;
std::vector<std::string> testImgPath = {
        "C:/Users/xgd/Pictures/yue.png",
        "C:/Users/xgd/Pictures/workspace/liang.png",
        "C:/Users/xgd/Pictures/workspace/xin.png"
};

std::vector<std::pair<int, int>> sizeConfig = {
        {1, 64},
        {2, 64},
        {2, 64},
        {2, 64},
        {2, 64},
        {3, 64},
        {3, 64},
};

inline torch::Tensor getOneSample(const std::string &_img_path) {
    auto cvImg = cv::imread(_img_path);
    cvImg.convertTo(cvImg, CV_32F, 1.0 / 255);
    std::cout << cvImg.cols << "x" << cvImg.rows << "x" << cvImg.channels() << std::endl;
    auto imgTensor = torch::from_blob(
            cvImg.data, {cvImg.cols, cvImg.rows, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous().unsqueeze(0);
    return std::move(imgTensor);
}

void trainClassifier(const std::string &_allClass, const std::string &_targetClass,
                     const std::string &_saveDir = "D:/", const std::string &_preloadPath = "") {
    srand(918);
    std::cout << std::fixed << std::setprecision(4);
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << "run on device:" << device << std::endl;
    // 超参数
    const int64_t batchSize = 240;
    const size_t maxEpochs = 20, lrDecayEpoch = 1;
    const double lr = 0.001, lrDecayRatio = 0.7;
    double curLr = lr;
    /**
     * 控制高斯分布的方差、颜色反转的概率
     */
    RC::revertColorProb = 0.5;
    RC::noiseParm.mean = 0;
    RC::noiseParm.stddev = 0;
    RC::shapeAttr.thickness = 2;
    CouchDataSet::setBatchImageSize(64, 64);
    const float maxGaussianVariance = 0.2;
    float dv = 0.00005;
    // 训练集
    auto trainSet = CouchDataSet(
            _allClass,
            _targetClass,
            CouchDataSet::kTrain).map(torch::data::transforms::Stack<>());
    auto trainSetSize = trainSet.size().value();
    auto trainSetLoader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(trainSet), batchSize);

    // 测试集
    auto testSet = CouchDataSet(
            _allClass,
            _targetClass,
            CouchDataSet::kTest).map(torch::data::transforms::Stack<>());
    auto testSetSize = testSet.size().value();
    auto testSetLoader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(testSet), batchSize);

    // 模型
    auto model = std::make_shared<Mv3Large>(CouchDataSet::GetNumOfClass());
    model->to(device);
    if (!_preloadPath.empty()) {
        torch::load(model, _preloadPath);
    }

    auto test = [&]() {
        std::cout << "Testing...\n";
        float tmp = RC::noiseParm.stddev;
        RC::revertColorProb = 0;
        RC::noiseParm.mean = 0;
        RC::noiseParm.stddev = 0;
        RC::shapeAttr.thickness = 2;
        // Test the model
        model->eval();
        torch::NoGradGuard no_grad;
        double running_loss = 0.0;
        size_t num_correct = 0;
        for (const auto &batch : *testSetLoader) {
            auto data = batch.data.to(device);
            auto target = batch.target.to(device);
            auto output = model->forward(data);
            auto loss = torch::nn::functional::cross_entropy(output, target);
            running_loss += loss.item<double>() * data.size(0);
            auto prediction = output.argmax(1);
            num_correct += prediction.eq(target).sum().item<int64_t>();
        }
        std::cout << "Testing finished!\n";
        auto test_accuracy = static_cast<double>(num_correct) / testSetSize;
        auto test_sample_mean_loss = running_loss / testSetSize;
        std::cout << "Testset - Loss: " << test_sample_mean_loss
                  << ", Accuracy: " << test_accuracy << std::endl;
        RC::revertColorProb = 0.5;// 还原
        RC::noiseParm.stddev = tmp;
        model->train(true);
    };
    std::cout << "Training...\n";
    torch::optim::Adam optimizer(
            model->parameters(), torch::optim::AdamOptions(curLr));
    for (size_t epoch = 0; epoch < maxEpochs; epoch++) {
        // Initialize running metrics
        double lossPerEpoch = 0.0;
        size_t top1CorrectPerEpoch = 0;
        size_t batchIndex = 0;
        for (auto &batch : *trainSetLoader) {
            // Transfer images and target labels to device
            auto data = batch.data.to(device), target = batch.target.to(device);
            // Forward pass
            auto output = model->forward(data);
            top1CorrectPerEpoch += output.argmax(1).eq(target).sum().item<int64_t>();
            // Calculate loss
            auto loss = torch::nn::functional::cross_entropy(output, target);
            // Backward pass and optimize
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
            // 统计部分
            // Update running loss
//            lossPerEpoch += loss.item<double>() * data.size(0);
            // Calculate prediction
//            auto prediction = output.argmax(1);
            // Update number of correctly classified samples
            // CPU 操作
            batchIndex++;
            if (batchIndex % 100 == 0) {
                size_t cfg_index = rand() % sizeConfig.size();
                RC::shapeAttr.thickness = sizeConfig[cfg_index].first;
                CouchDataSet::setBatchImageSize(sizeConfig[cfg_index].second, sizeConfig[cfg_index].second);
                if (RC::noiseParm.stddev < maxGaussianVariance) {
                    RC::noiseParm.stddev += dv;
                }
                float sampleSum = batchIndex * batchSize;
                std::cout << "Epoch [" << (epoch + 1) << "/" << maxEpochs
                          << "], Batch [" << batchIndex
                          // << "], TrainLoss: " << lossPerEpoch / sampleSum
                          << "], Acc: " << top1CorrectPerEpoch / sampleSum << "\n";
                std::string model_save_path = _saveDir + "mv3-epoch-" + std::to_string(epoch % 10) + ".pth";
                torch::save(model, model_save_path);
            }
        }
        float sampleSum = batchIndex * batchSize;
        if (top1CorrectPerEpoch / sampleSum > 0.9) {
            test();
        }
        // Decay learning rate
        if ((epoch + 1) % lrDecayEpoch == 0) {
            curLr *= lrDecayRatio;
            static_cast<torch::optim::AdamOptions &>(optimizer.param_groups().front()
                    .options()).lr(curLr);
        }
    }
    std::cout << "Training finished!\n\n";
}

void testClassifier(const std::string &_preloadPath) {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    std::cout << device << std::endl;
    auto model = std::make_shared<Mv3Large>(6946);
    torch::load(model, _preloadPath);
    model->to(device);
    model->eval();
    while (model.get()) {
        Timer timer;
        timer.start();
        auto input = getOneSample(
                testImgPath[rand() % testImgPath.size()]).to(device);
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
    try {
        trainClassifier(
                "C:/Users/xgd/source/COCR/data/couch/couch-gbk.txt",
                "C:/Users/xgd/source/COCR/data/couch/couch-gbk-target.txt",
                "D:/");
//        testClassifier("C:/Users/xgd/source/COCR/cache/gb/mv3-epoch-6.pth");
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(0);
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