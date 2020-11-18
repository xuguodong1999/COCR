#ifndef _TORCH_CLASSIFIER_HPP_
#define _TORCH_CLASSIFIER_HPP_

#include <torch/nn/modules/container/sequential.h>
#include <memory>
#include <vector>
#include <utility>

/**
 * 将分类器看作三个部分：输入-若干次下采样-分类
 * 表达为 Sequential、Sequential[]、Sequential
 * 以 Sequential 为粒度提供权重冻结、解冻、分离保存、分离加载
 */
class BaseClassifier {
    std::string name;
protected:
    torch::nn::Sequential layerIn;
    std::vector<torch::nn::Sequential> bnecks;
    torch::nn::Sequential layerOut;
public:

    BaseClassifier(torch::nn::Sequential _layerIn,
                   std::vector<torch::nn::Sequential> _bnecks,
                   torch::nn::Sequential _layerOut);

    void freezeInLayer(bool _freeze = true);

    void freezeBneck(const size_t &i, bool _freeze = true);

    void freezeOutLayer(bool _freeze = true);

    const std::string &getName() const;

    void setName(const std::string &name);

    virtual void saveClassifier(const std::string &_saveDir,
                                const std::string &_inPrefix = "in",
                                const std::string &_bneckPrefix = "bneck",
                                const std::string &_outPrefix = "out");

    /**
     * 传入空值表示不加载，传入错误值直接退出
     * @param _saveDir 权重所在目录
     * @param _inPrefix 前置滤波器的文件名
     * @param _bneckPrefix 下采样滤波器的文件名
     * @param _outPrefix 全连接分类部分的文件名
     * @param _device cpu or cuda
     */
    virtual void loadClassifier(const std::string &_saveDir,
                                const std::string &_inPrefix = "in",
                                const std::string &_bneckPrefix = "bneck",
                                const std::string &_outPrefix = "out",
                                const torch::Device &_device = torch::kCPU);

    /**
     * You must implement these functions when using BaseClassifier
     */
    virtual torch::Tensor forward(torch::Tensor input) = 0;

    virtual void registerModule() = 0;

};

#endif//_TORCH_CLASSIFIER_HPP_
