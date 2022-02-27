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
#include <concurrentqueue/blockingconcurrentqueue.h>

const torch::DeviceType runtime_device = torch::kCUDA;
const size_t batch_size = 64;
const size_t LAYER_BASE_WIDTH = 128;
const size_t cached_batch = 24;
double learning_rate = 0.001;
const size_t num_epochs = 20;
const size_t worker_num = 16;
const size_t DATA_SET_SIZE = 1024;
const size_t width = 320, height = 320, out_channel = 10, in_channel = 1;
size_t epoch = 0;

using data_item_type = std::pair<torch::Tensor, torch::Tensor>;

inline data_item_type get_data_with_samples(const int64_t &sample_num, const torch::DeviceType device = torch::kCPU) {
    auto input = torch::ones({sample_num, in_channel, width, height})
            .toType(torch::kFloat32).to(device, true);
    auto label = torch::ones({sample_num, out_channel})
            .toType(torch::kFloat32).to(device, true);
    return {input, label};
}

class ElsDataset : public torch::data::datasets::Dataset<ElsDataset> {
    torch::Tensor input, label;
public:
    ElsDataset() {
        auto[a, b] = get_data_with_samples(1);
        input = a.squeeze(0);
        label = b.squeeze(0);
    }

    torch::data::Example<> get(size_t index) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
    inline static const int64_t BASE_SIZE = LAYER_BASE_WIDTH;
    torch::nn::Conv2d conv1, conv2;
    torch::nn::Conv2d fc1, fc2, fc3;

public:
    ElsNetImpl() : conv1(conv_opt(in_channel, BASE_SIZE, 3, 2, 1)),
                   conv2(conv_opt(BASE_SIZE, BASE_SIZE * 2, 3, 2, 1)),
                   fc1(conv_opt(BASE_SIZE * 2, BASE_SIZE * 4, 1)),
                   fc2(conv_opt(BASE_SIZE * 4, BASE_SIZE * 8, 1)),
                   fc3(conv_opt(BASE_SIZE * 8, out_channel, 1)) {
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

// just forward and backward, ideal case
void test_with_fixed_data();

// load tensor from torch::data::dataset, standard usage
void test_with_torch_dataset();

// load data from self-defined consumer-producer, preload data into gpu
void test_with_queue();

int main(int argc, char **argv) {
//    test_with_fixed_data();
//    test_with_torch_dataset();
    test_with_queue();
    return 0;
}

void test_with_fixed_data() {
    auto model = ElsNet();
    auto optimizer = torch::optim::Adam(
            model->parameters(), torch::optim::AdamOptions(learning_rate));
    model->to(runtime_device);

    const auto[single_input, single_label]=get_data_with_samples(batch_size);
    auto input = single_input.to(runtime_device, true);
    auto label = single_label.to(runtime_device, true);
    while (true) {
        auto output = model->forward(input);
        auto loss = torch::nn::functional::cross_entropy(output, label);
        optimizer.zero_grad();
        loss.backward();
        optimizer.step();
        if (++epoch > num_epochs * DATA_SET_SIZE / batch_size) break;
    }
}

void test_with_torch_dataset() {
    auto train_dataset = ElsDataset().map(torch::data::transforms::Stack<>());

    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(train_dataset), torch::data::DataLoaderOptions()
                    .batch_size(batch_size).workers(worker_num));

    auto model = ElsNet();
    auto optimizer = torch::optim::Adam(
            model->parameters(), torch::optim::AdamOptions(learning_rate));
    model->to(runtime_device, true);
    while (true) {
        double running_loss = 0.0;
        for (const auto &[data, target]: *train_loader) {
            auto input = data.to(runtime_device, true);
            auto label = target.to(runtime_device, true);
            auto output = model->forward(input);
            auto loss = torch::nn::functional::cross_entropy(output, label);
            running_loss += loss.item<double>() * data.size(0);
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }
        std::cout << "running_loss=" << running_loss << std::endl;
        learning_rate /= 0.999;
        optimizer.param_groups().front().options().set_lr(learning_rate);
        if (++epoch > num_epochs) break;
    }
}

void test_with_queue() {
    moodycamel::BlockingConcurrentQueue<data_item_type> q;
    std::mutex mtx; // 全局互斥锁.
    std::condition_variable cv; // 全局条件变量.
    bool ready = false; // 全局标志位.
    bool quit = false;

    std::thread producer([&]() {
        std::unique_lock<std::mutex> lck(mtx);
        while (true) {
            while (!ready)
                cv.wait(lck);
            if (quit) break;
            while (q.size_approx() < cached_batch) {
                auto[batch_input, batch_label]=get_data_with_samples(
                        batch_size, runtime_device);
                q.enqueue({batch_input, batch_label});
            }
            ready = false;
            if (quit) break;
        }
    });
    std::thread consumer([&]() {
        auto model = ElsNet();
        auto optimizer = torch::optim::Adam(
                model->parameters(), torch::optim::AdamOptions(learning_rate));
        model->to(runtime_device);
        while (true) {
            if (q.size_approx() < cached_batch / 3) {
                std::unique_lock<std::mutex> lck(mtx);
                ready = true;
                cv.notify_all();
            }
            data_item_type item;
            q.wait_dequeue(item);
            auto&[input, label]=item;
            auto output = model->forward(input);
            auto loss = torch::nn::functional::cross_entropy(output, label);
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
            if (++epoch > num_epochs * DATA_SET_SIZE / batch_size) {
                quit = true;
                std::unique_lock<std::mutex> lck(mtx);
                ready = true;
                cv.notify_all();
                break;
            }
        }
    });
    producer.join();
    consumer.join();
}
