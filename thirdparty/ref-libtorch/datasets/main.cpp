#include <torch/data/dataloader.h>
#include <torch/data/transforms/stack.h>
#include "dataSet.h"

int main(int argc, char *argv[]) {
    int batch_size = 2;
    std::string image_dir = "~/datasets/hymenoptera_data\\train";
    auto mdataset = myDataset(image_dir, ".jpg").map(torch::data::transforms::Stack<>());
    auto mdataloader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(std::move(mdataset),
                                                                                           batch_size);
    for (auto &batch: *mdataloader) {
        auto data = batch.data;
        auto target = batch.target;
        std::cout << data.sizes() << target;
    }

    return 0;
}
