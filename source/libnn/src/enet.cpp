#include "nn/enet.h"
#include "nn/module.h"

#include <torch/torch.h>

using namespace torch;
using namespace torch::nn;
// TODO: not implemented yet

EfficientNet::EfficientNet(int _numOfClass, const std::string &_cfg) : BaseClassifier(
        Sequential(
                Conv2d(Conv2dOptions(
                        3, kw(_cfg, 32), {3, 3})
                               .stride({2, 2}).bias(false)),
                BatchNorm2d(BatchNorm2dOptions(kw(_cfg, 32)))),
        {},// TODO: Add Layers to Bnecks by loop in sBneckConfig
        Sequential()) {

    /**
     * enet 的 expandRatio 表示层内堆叠的宽度增长率
     * mv3 的 expandRatio 表示一个3-3块内中间层的宽度
     */
    for (auto&[repeat, kernelSize, stride,
        expandSize, inSize, outSize, reduction]: sBneckConfig) {
        Sequential bneck;
        for (int i = 0; i < repeat; i++) {
            // FIXME: Check the difference between mobilenetv3 and enet module setting
//            bneck->push_back(MBConv(
//                    inSize, outSize, kernelSize, stride, reduction));
        }
        bnecks.push_back(std::move(bneck));
    }
}


torch::Tensor EfficientNet::forward(torch::Tensor x) {
    return torch::Tensor();
}

void EfficientNet::registerModule() {

}
