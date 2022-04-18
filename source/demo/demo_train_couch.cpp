#include "base/timer.h"
#include "nn/util.h"
#include "nn/mv3.h"
#include "dataset_cifar.h"
#include <torch/torch.h>

void transferCouch2Cifar(torch::Device &device) {
    auto model = std::make_shared<Mv3Large>(100);
//    model->setName("gb-6946");
    model->to(device);
//    model->loadClassifier(
//            "~/source/COCR/cache/weights/gb_classification/mv3-epoch-0", "in", "bneck", "", device);
    model->setName("cifar-100");
    torch::load(model, "D:/cifar-100-epoch-0.pth", device);
//    model->freezeInLayer(true);
//    for (size_t i = 0; i < 5; i++) {
//        model->freezeBneck(i, true);
//    }

    // Hyper parameters
    const int64_t batch_size = 128;
    const size_t num_epochs = 200;
    const double learning_rate = 0.0001;
    const size_t learning_rate_decay_frequency = 20;
    const double learning_rate_decay_factor = 0.5;

    const std::string cifar100_root = "~/source/COCR/cache/data/cifar-100";
    const std::string cifar10_root = "~/source/COCR/cache/data/cifar-10";
    auto trainSet = CifarDataSet(
            CifarType::CIFAR100, cifar100_root, CifarDataSet::kTrain)
            .map(torch::data::transforms::Stack<>());
    auto num_train_samples = trainSet.size().value();
    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(trainSet), batch_size);

    auto testSet = CifarDataSet(CifarType::CIFAR100, cifar100_root, CifarDataSet::kTest).map(
            torch::data::transforms::Stack<>());
    std::cout << testSet.size().value() << std::endl;
    auto num_test_samples = testSet.size().value();
    auto test_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(testSet), batch_size);
    auto test = [&]() -> void {
        std::cout << "Testing...\n";
        // Test the model
        model->eval();

        double running_loss = 0.0;
        size_t num_correct = 0;

        for (const auto &batch : *test_loader) {
            auto data = batch.data.to(device);
            auto target = batch.target.to(device);

            auto output = model->forward(data);
//            torch::nn::CTCLoss
            auto loss = torch::nn::functional::nll_loss(output, target);
//            auto loss = torch::nn::functional::cross_entropy(output, target);
            running_loss += loss.item<double>() * data.size(0);

            auto prediction = output.argmax(1);
            num_correct += prediction.eq(target).sum().item<int64_t>();
        }

        std::cout << "Testing " << num_test_samples << " finished!\n";

        auto test_accuracy = static_cast<double>(num_correct) / num_test_samples;
        auto test_sample_mean_loss = running_loss / num_test_samples;

        std::cout << "Testset - Loss: " << test_sample_mean_loss << ", Accuracy: " << test_accuracy << '\n';
        model->train(true);
    };
    torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(learning_rate));

    // Set floating point output precision
    std::cout << std::fixed << std::setprecision(6);

    auto current_learning_rate = learning_rate;
    std::cout << "Training...\n";
    bool isFrozen = true;
    // Train the model
    for (size_t epoch = 0; epoch != num_epochs; ++epoch) {
        // Initialize running metrics
//        double running_loss = 0.0;
        size_t num_correct = 0;
        for (auto &batch : *train_loader) {
            // Transfer images and target labels to device
            auto data = batch.data.to(device);
            auto target = batch.target.to(device);
            // Forward pass
            auto output = model->forward(data);
            // Calculate prediction
            auto prediction = output.argmax(1);
            // Update number of correctly classified samples
            num_correct += prediction.eq(target).sum().item<int64_t>();
            // Calculate loss
            auto loss = torch::nn::functional::cross_entropy(output, target);
            // Update running loss
//            running_loss += loss.item<double>() * data.size(0);
            // Backward pass and optimize
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }
        // Decay learning rate
        if ((epoch + 1) % learning_rate_decay_frequency == 0) {
            current_learning_rate *= learning_rate_decay_factor;
            static_cast<torch::optim::AdamOptions &>(optimizer.param_groups().front()
                    .options()).lr(current_learning_rate);
        }
//        auto sample_mean_loss = running_loss / num_train_samples;
        auto accuracy = static_cast<double>(num_correct) / num_train_samples;
        std::string model_save_path = "D:/" + model->getName() + "-epoch-"
                                      + std::to_string(epoch % 10) + ".pth";
        torch::save(model, model_save_path);
        std::cout << "Epoch [" << (epoch + 1) << "/" << num_epochs
                  //                  << "], Trainset - Loss: " << sample_mean_loss
                  << "], Trainset - Accuracy: " << accuracy << '\n';
        if (accuracy > 0.8) {
            test();
            if (isFrozen) {
                isFrozen = false;
                model->freezeInLayer(false);
                for (size_t i = 0; i < 5; i++) {
                    model->freezeBneck(i, false);
                }
                static_cast<torch::optim::AdamOptions &>(optimizer.param_groups().front()
                        .options()).lr(current_learning_rate);
            }
        }
    }

    std::cout << "Training finished!\n\n";
}

int main() {
    try {
        torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
        transferCouch2Cifar(device);
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
//    model->saveClassifier("D:/","in","bneck","out");
    model->loadClassifier("D:/", "in", "bneck", "out", device);
    model->freezeInLayer(true);
    model->eval();
    Timer timer;
    timer.start();
    auto input = loadImgTensor("~/source/COCR/cache/simple_test/liang.png");
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
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}