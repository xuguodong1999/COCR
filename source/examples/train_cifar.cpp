
#include "base/timer.h"
#include "nn/util.h"
#include "nn/mv3.h"
#include "nn/datasets/cifar.h"
#include <torch/torch.h>
#include <ATen/autocast_mode.h>

const char *CIFAR100_ROOT = DATASET_PATH "/cifar100";
const char *CIFAR10_ROOT = DATASET_PATH "/cifar10";

struct TrainConfig {
    int64_t BATCH_SIZE = 512;
    size_t MAX_EPOCHS = 200;
    double LR_BEGIN = 0.001, LR_CURRENT;
    size_t LR_CHANGE_TIMES = 10;
    double LR_DECAY_FACTOR = 0.8;
    size_t WORKER_NUM = 12;
    size_t WIDTH = 96;

    TrainConfig() {
        std::cout << std::fixed << std::setprecision(6);
    }
};

void train_cifar(torch::Device &_device, const CifarType &_cifarType) {
    int num_classes(10);
    std::string task_name("cifar10"), data_root(CIFAR10_ROOT);
    if (_cifarType == CifarType::CIFAR100) {
        num_classes = 100;
        task_name = "cifar100";
        data_root = CIFAR100_ROOT;
    }
    auto model = std::make_shared<Mv3Large>(num_classes, 1.0);
//    torch::save(model, "/tmp/mv3-large.pth");
    model->setName(task_name + "-mv3large");
    model->to(_device);
//    auto imgTensor = torch::zeros({1024, 3, 32,32}).to(_device);
//    while (true){
//        model->forward(imgTensor);
//    }
    TrainConfig cfg;
    auto make_dataset = [&](const CifarDataSet::Mode &_mode) {
        auto dataset = CifarDataSet(_cifarType, data_root, _mode, cfg.WIDTH, cfg.WIDTH)
                .map(torch::data::transforms::Stack<>());
        auto num_samples = dataset.size().value();
        auto data_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
                std::move(dataset), torch::data::DataLoaderOptions()
                        .batch_size(cfg.BATCH_SIZE).workers(cfg.WORKER_NUM));
        return std::make_pair(num_samples, std::move(data_loader));
    };

    auto[num_train_samples, train_loader]=make_dataset(CifarDataSet::Mode::kTrain);
    auto[num_test_samples, test_loader]=make_dataset(CifarDataSet::Mode::kTest);

    cfg.LR_CURRENT = cfg.LR_BEGIN;
    torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(cfg.LR_CURRENT));
//    torch::optim::SGD optimizer(model->parameters(), torch::optim::SGDOptions(cfg.LR_CURRENT));
    std::cout << "Training...\n";
    // Train the model
    for (size_t epoch = 1; epoch <= cfg.MAX_EPOCHS; epoch++) {
        double running_loss = 0.0;
        size_t num_correct = 0;
        for (auto &batch: *train_loader) {
            // Transfer images and target labels to device
            auto data = batch.data.to(_device);
            auto target = batch.target.to(_device);
            // Forward pass
            torch::Tensor output;
            {  // mixed precision link amp
                at::autocast::set_enabled(true);
                output = model->forward(data);
                at::autocast::clear_cache();
                at::autocast::set_enabled(false);
            }
            // Calculate prediction
            auto prediction = output.argmax(1);
            // Update number of correctly classified samples
            num_correct += prediction.eq(target).sum().item<int64_t>();
            // Calculate loss
            auto loss = torch::nn::functional::nll_loss(output, target);
            // Update running loss
            running_loss += loss.item<double>() * data.size(0);
            // Backward pass and optimize
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }
        // Decay learning rate
        if (epoch % cfg.LR_CHANGE_TIMES == 0) {
            cfg.LR_CURRENT *= cfg.LR_DECAY_FACTOR;
            static_cast<torch::optim::AdamOptions &>(optimizer.param_groups().front().options()).lr(cfg.LR_CURRENT);
//            static_cast<torch::optim::SGDOptions &>(optimizer.param_groups().front().options()).lr(cfg.LR_CURRENT);
        }
        double acc = static_cast<double>(num_correct) / num_train_samples;
        std::cout << "epoch-" << epoch << "\ntrain loss = "
                  << running_loss / num_train_samples << " ,acc = "
                  << acc << '\n';
        if (acc > 0.5) {
//            std::cout << "Testing...\n";
            model->eval();
            double test_loss = 0.0;
            size_t test_correct = 0;
            for (const auto &batch: *test_loader) {
                auto data = batch.data.to(_device);
                auto target = batch.target.to(_device);
                auto output = model->forward(data);
                auto loss = torch::nn::functional::nll_loss(output, target);
                test_loss += loss.item<double>() * data.size(0);
                auto prediction = output.argmax(1);
                test_correct += prediction.eq(target).sum().item<int64_t>();
            }
//            std::cout << "Testing on " << num_test_samples << " samples finished!\n";
            auto test_accuracy = static_cast<double>(test_correct) / num_test_samples;
            auto test_sample_mean_loss = test_loss / num_test_samples;
            std::cout << "test loss = " << test_sample_mean_loss << " ,acc = " << test_accuracy << '\n';
            model->train(true);
        }
    }

    std::cout << "Training finished!\n\n";

}

int main() {
    try {
        torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
        train_cifar(device, CifarType::CIFAR100);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
    return 0;
}

void test() {
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
    auto model = std::make_shared<Mv3Large>(6946);
    model->setName("gb-6946");
    model->to(device);
//    torch::load(model, WEIGHTS_DIR + "/gb_classification/mv3-epoch-0.pth", device);
//    model->saveClassifier("/tmp/","in","bneck","out");
    model->loadClassifier("/tmp/", "in", "bneck", "out", device);
    model->freezeInLayer(true);
    model->eval();
    Timer timer;
    timer.start();
    auto input = NNUtil::loadImgTensor("/tmp/liang.png");
    auto output = model->forward(input.to(device));
    int topk = 5;
    auto prediction = output.topk(topk, 1);
    std::cout << std::get<0>(prediction).squeeze() << std::endl;
    auto indices = std::get<1>(prediction).squeeze(0);
    for (int i = 0; i < topk; i++) {
        std::cout << indices[i].item().toInt() << std::endl;
    }
    timer.stop();
    output.print();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}