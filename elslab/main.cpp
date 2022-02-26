#include <torch/serialize.h>
#include <torch/types.h>
#include <torch/cuda.h>
#include <torch/nn/module.h>
#include <torch/nn/modules/conv.h>
#include <torch/data/datasets/base.h>
#include <torch/data/dataloader.h>
#include <torch/data/samplers/sequential.h>
#include <torch/optim/adam.h>
#include <torch/data/transforms/stack.h>
#include <torch/data/datasets/map.h>
#include <torch/nn/functional/loss.h>

using data_item_type = std::pair<torch::Tensor, torch::Tensor>;

class ElsDataset : public torch::data::datasets::Dataset<ElsDataset> {
    inline static const size_t DATA_SET_SIZE = 10240;
    torch::Tensor input, label;
public:
    ElsDataset() {
        input = torch::ones(
                {1, 416, 640}, torch::TensorOptions().requires_grad(false))
                        .toType(torch::kFloat32) * 0.2022;
        label = torch::ones(
                {10}, torch::TensorOptions().requires_grad(false))
                        .toType(torch::kFloat32) * 0.1;
    }

    torch::data::Example<> get(size_t index) override {
//        auto input = torch::ones(
//                {1, 416, 640}, torch::TensorOptions().requires_grad(false))
//                             .toType(torch::kFloat32) * 0.2022;
//        auto label = torch::ones(
//                {10}, torch::TensorOptions().requires_grad(false))
//                             .toType(torch::kFloat32) * 0.1;
        return {input, label};
    }

    torch::optional<size_t> size() const override {
        return DATA_SET_SIZE;
    }
};

inline torch::nn::Conv2dOptions conv_opt(
        const int64_t &channel_in, const int64_t &channel_out, const int64_t kernel_size = 3,
        const int64_t &stride = 1, const int64_t &padding = 0) {
    return torch::nn::Conv2dOptions(channel_in, channel_out, kernel_size)
            .padding(padding).stride(stride);
}

class ElsNetImpl : public torch::nn::Module {
    inline static const int64_t BASE_SIZE = 16;
    torch::nn::Conv2d conv1, conv2;
    torch::nn::Conv2d fc1, fc2, fc3;

public:
    ElsNetImpl() : conv1(conv_opt(1, BASE_SIZE, 3, 2, 1)),
                   conv2(conv_opt(BASE_SIZE, BASE_SIZE * 2, 3, 2, 1)),
                   fc1(conv_opt(BASE_SIZE * 2, BASE_SIZE * 4, 1)),
                   fc2(conv_opt(BASE_SIZE * 4, BASE_SIZE * 8, 1)),
                   fc3(conv_opt(BASE_SIZE * 8, 10, 1)) {
        register_module("conv1", conv1);
        register_module("conv2", conv2);
        register_module("fc1", fc1);
        register_module("fc2", fc2);
        register_module("fc3", fc3);
    }

    torch::Tensor forward(const torch::Tensor &x) {
        auto out = x;
        out = torch::relu(conv1->forward(out));
        out = torch::relu(conv2->forward(out));
        out = torch::adaptive_avg_pool2d(out, {1, 1});
        out = torch::relu(fc1->forward(out));
        out = torch::relu(fc2->forward(out));
        out = fc3->forward(out);
        return out.squeeze(-1).squeeze(-1);
    }

};

TORCH_MODULE(ElsNet);

int main(int argc, char **argv) {
    const auto device = (torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);

    const size_t batch_size = 512;
    double learning_rate = 0.001;
    const size_t num_epochs = 200;
    const size_t worker_num = 16;
    size_t epoch = 0;

    auto train_dataset = ElsDataset().map(torch::data::transforms::Stack<>());
    auto test_dataset = ElsDataset().map(torch::data::transforms::Stack<>());

    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(train_dataset), torch::data::DataLoaderOptions()
                    .batch_size(batch_size).workers(worker_num));

    auto test_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(test_dataset), torch::data::DataLoaderOptions()
                    .batch_size(batch_size).workers(worker_num));

    auto model = ElsNet();
    auto optimizer = torch::optim::Adam(
            model->parameters(), torch::optim::AdamOptions(learning_rate));
    model->to(device);
    auto single_input = torch::ones(
            {batch_size, 1, 416, 640}, torch::TensorOptions().requires_grad(false))
            .toType(torch::kFloat32).to(device);;
    auto single_label = torch::ones(
            {batch_size, 10}, torch::TensorOptions().requires_grad(false))
            .toType(torch::kFloat32).to(device);

    while (true) {
        auto input = single_input.to(device);
        auto output = model->forward(input);
        auto label = single_label.to(device);
        auto loss = torch::nn::functional::cross_entropy(output, label);
        optimizer.zero_grad();
        loss.backward();
        optimizer.step();
        // 0ms-100%
        // 100ms-97%~100%
        // 150ms-50%~70%
        // conclusion: torch::data::datasets have bugs for my online data-generation usage
        // must write it alone
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (++epoch > num_epochs) break;
    }
    while (true) {
        double running_loss = 0.0;
        for (const auto &[data, target]: *train_loader) {
            auto input = data.to(device);
            auto output = model->forward(input);
            auto label = target.to(device);
            auto loss = torch::nn::functional::cross_entropy(output, label);
//            running_loss += loss.item<double>() * data.size(0);
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }
        std::cout << "running_loss=" << running_loss << std::endl;
        learning_rate /= 0.999;
        optimizer.param_groups().front().options().set_lr(learning_rate);
        if (++epoch > num_epochs) break;
    }

    return 0;
}