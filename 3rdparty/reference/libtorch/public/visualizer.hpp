#pragma once

#include <ref_torch_public_export.h>
#include <string>
#include <tuple>
#include <vector>
#include <utility>
// For External Library
#include <torch/types.h>
#include <opencv2/opencv.hpp>


// -----------------------------------
// namespace{visualizer}
// -----------------------------------
namespace visualizer {

    // Function Prototype
    REF_TORCH_PUBLIC_EXPORT
    void save_image(const torch::Tensor image, const std::string path, const std::pair<float, float> range = {0.0, 1.0},
                    const size_t cols = 8, const size_t padding = 2, const size_t bits = 8);

    REF_TORCH_PUBLIC_EXPORT
    void save_label(const torch::Tensor label, const std::string path,
                    const std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> label_palette,
                    const size_t cols = 8, const size_t padding = 2);

    REF_TORCH_PUBLIC_EXPORT
    cv::Mat draw_detections(const torch::Tensor image, const std::tuple<torch::Tensor, torch::Tensor> label,
                            const std::vector<std::string> class_names,
                            const std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> label_palette,
                            const std::pair<float, float> range = {0.0, 1.0});

    REF_TORCH_PUBLIC_EXPORT
    cv::Mat draw_detections_des(const torch::Tensor image, const std::tuple<torch::Tensor, torch::Tensor> label,
                                const torch::Tensor prob, const std::vector<std::string> class_names,
                                const std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> label_palette,
                                const std::pair<float, float> range = {0.0, 1.0});

    // -----------------------------------
    // namespace{visualizer} -> class{graph}
    // -----------------------------------
    class REF_TORCH_PUBLIC_EXPORT graph {
    private:
        bool flag;
        std::string dir, data_dir;
        std::string gname;
        std::string graph_fname, data_fname;
        std::vector<std::string> label;
    public:
        graph() {}

        graph(const std::string dir_, const std::string gname_, const std::vector<std::string> label_);

        void plot(const float base, const std::vector<float> value);
    };

}