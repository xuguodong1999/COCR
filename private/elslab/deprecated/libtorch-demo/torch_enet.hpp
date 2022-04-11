#ifndef _TORCH_ENET_HPP_
#define _TORCH_ENET_HPP_

//#include "torch_classifier.hpp"
//#include <vector>
//#include <unordered_map>
//
//class EfficientNet : public torch::nn::Module, public BaseClassifier {
//    /**
//     * 缩放系数
//     */
//    inline static std::unordered_map<std::string, std::tuple<float, float, int, float>>
//            sCoeffConfig = {// kw, kh, resolution, dropout
//            {"b0", {1.0, 1.0, 224, 0.2}},
//            {"b1", {1.0, 1.1, 240, 0.2}},
//            {"b2", {1.0, 1.0, 224, 0.2}},
//            {"b3", {1.2, 1.4, 300, 0.3}},
//            {"b4", {1.4, 1.8, 380, 0.4}},
//            {"b5", {1.6, 2.2, 456, 0.4}},
//            {"b6", {1.8, 2.6, 528, 0.5}},
//            {"b7", {2.0, 3.1, 600, 0.5}}
//    };
//    /**
//     * bneck 基础数值
//     */
//    inline static std::vector<std::tuple<int, int, int, int, int, int, float>> sBneckConfig = {
//            // num_repeat?,kernel_size,stride,expand_ratio，in,out,se_reduction
//            {1, 3, 1, 1, 32,  16,  0.25},//16*1=16
//            {2, 3, 2, 6, 16,  24,  0.25},//24*2*2=96
//            {2, 5, 2, 6, 24,  40,  0.25},// 144
//            {3, 3, 2, 6, 40,  80,  0.25},
//            {3, 5, 1, 6, 80,  112, 0.25},
//            {4, 5, 2, 6, 112, 192, 0.25},
//            {1, 3, 1, 6, 192, 320, 0.25}
//    };
//    /**
//     * 获取宽度缩放结果
//     */
//    inline static auto kw = [](const std::string &_cfg, int _base) -> float {
//        return std::round(_base * std::get<0>(sCoeffConfig[_cfg]));
//    };
//    /**
//     * 获取高度缩放结果
//     */
//    inline static auto kh = [](const std::string &_cfg, int _base) -> float {
//        return std::round(_base * std::get<1>(sCoeffConfig[_cfg]));
//    };
//    /**
//     * 训练用分辨率
//     */
//    inline static auto resolution = [](const std::string &_cfg) -> int {
//        return std::get<2>(sCoeffConfig[_cfg]);
//    };
//    /**
//     * 正则化超参
//     */
//    inline static auto dropout = [](const std::string &_cfg) -> float {
//        return std::get<3>(sCoeffConfig[_cfg]);
//    };
//public:
//    EfficientNet(int _numOfClass, const std::string &_cfg);
//
//    torch::Tensor forward(torch::Tensor x);
//
//    void registerModule();
//};

#endif//_TORCH_ENET_HPP_
