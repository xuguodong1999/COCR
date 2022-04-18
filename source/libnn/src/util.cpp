#include "nn/util.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <torch/torch.h>

torch::Tensor NNUtil::loadImgTensor(const std::string &_imgPath,
                                    const int &_width, const int &_height) {
    auto cvImg = cv::imread(_imgPath);
    if (3 != cvImg.channels()) {
        std::cerr << _imgPath << " is not a 3-channel img" << std::endl;
        exit(-1);
    }
    if (_width > 0 && _height > 0 && (_width != cvImg.cols || _height != cvImg.rows)) {
        cv::resize(cvImg, cvImg, cv::Size(_width, _height));
    }
    cvImg.convertTo(cvImg, CV_32F, 1.0 / 255);
    std::cout << "img.size=" << cvImg.cols << "x" << cvImg.rows
              << "x" << cvImg.channels() << std::endl;
    auto imgTensor = torch::from_blob(
            cvImg.data, {cvImg.cols, cvImg.rows, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous().unsqueeze(0);
    return std::move(imgTensor);
}
