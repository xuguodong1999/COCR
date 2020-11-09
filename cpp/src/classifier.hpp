#ifndef _CLASSIFIER_HPP_
#define _CLASSIFIER_HPP_
#include <memory>
#include <string>
#include <torch/nn/module.h>
enum ClassifierJob {
    Cifar100,
    Couch
};
/**
 * 要控制哪些参数？
 * 什么数据集：枚举类型
 * 什么模型：指针
 * 怎么加载：1、全新开始；2、继续训练；3、冻结卷积核的迁移训练
 * 超参数控制：大小、学习率策略、优化器
 * 特殊需求-等待时间：自动控制，白天拉满，晚上半满
 */
class Classifier {
    std::shared_ptr<torch::nn::Module> model;
    std::string mSaveDir,mLogDir;
    int mMaxEpoch,mBeginEpoch,mCurEpoch;
    double mCurLr;
public:
    void setConfig();
    void startTrain();
    void restartTrain();
    void transferTrain();

};

#endif//_CLASSIFIER_HPP_