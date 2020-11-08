#ifndef _COUCH_DATASET_HPP_
#define _COUCH_DATASET_HPP_

#include "config.hpp"
#include "couch.hpp"
#include <torch/torch.h>
#include <unordered_map>


// img-label
// based on: https://github.com/pytorch/pytorch/blob/master/torch/csrc/api/include/torch/data/datasets/mnist.h.
/**
 * 矢量转位图的加载器
 * 配置文件定义如下：
 * ①字符映射表：xxx.txt，每一行格式：实际编码-整数编码
 * ②被分类字符：xxx.txt，每一行格式：实际编码
 * meta data 加载进 CouchItem 的静态变量
 *
 */
class CouchDataSet : public torch::data::datasets::Dataset<CouchDataSet> {
    inline static std::vector<std::pair<size_t, size_t>> sGlobal2GridVec;
    inline static std::unordered_map<int, int> sCastTable;

    static void InitMap(const std::string &_allClass, const std::string &_targetClass);

    inline static bool isConfigLoaded = false;

    inline static int batchWidth = 64;
    inline static int batchHeight = 64;

    inline static float trainDivRatio = 10.0 / 11;

public:

    static void setBatchImageSize(int trainWidth, int trainHeight);

    static size_t GetNumOfClass();

    void shuffle();

    enum Mode {
        kTrain, kTest
    };

    CouchDataSet(const std::string &_allClass, const std::string &_targetClass,
                 const Mode &_mode = kTest);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrainMode() const noexcept;

private:
    /**
     * <>
     */
    std::vector<std::pair<size_t, size_t>> dataset;
    const Mode mode;
};

#endif//_COUCH_DATASET_HPP_