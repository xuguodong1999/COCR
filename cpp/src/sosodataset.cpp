#include "sosodataset.hpp"

std::pair<cv::Mat, std::tuple<float, float, float>>
padCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight) {
    int w = _img.cols;
    int h = _img.rows;
    float kw = (float) _newWidth / w, kh = (float) _newHeight / h;
    float k = std::min(kw, kh);
    int newWidth = k * w, newHeight = k * h;
    cv::Mat ret;
//    std::cout << w << "," << h << "," << k << "," << std::endl;
    cv::resize(_img, ret, cv::Size(newWidth, newHeight));
    int dw = std::max(0, _newWidth - newWidth), dh = std::max(0, _newHeight - newHeight);
    cv::copyMakeBorder(ret, ret,
                       dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
                       cv::BORDER_CONSTANT);
//    std::cout<<dw<<","<<dh<<","<<ret.cols<<","<<ret.rows<<std::endl;
    return std::make_pair(std::move(ret), std::make_tuple(k, dw / 2.f, dh / 2.f));
}

std::vector<YoloDataType> SosoDataSet::readYoloData(const std::string &_list_txt) {
    std::vector<YoloDataType> dataSet;
    std::ifstream ifsm(_list_txt);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << _list_txt << std::endl;
        exit(-1);
    }
    std::string image_path, label_path;
    while (std::getline(ifsm, image_path)) {
        image_path = trim(image_path);
        if (image_path.empty()) {
            continue;
        }
        label_path = replaceSubStr(replaceSubStr(
                image_path, ImageDir, LabelDir), ImageSuffix, LabelSuffix);
        if (!std::filesystem::exists(image_path)) {
            std::cerr << image_path << " not exists" << std::endl;
            exit(-1);
        }
        std::ifstream ifsm_label(label_path);
        if (!ifsm_label.is_open()) {
            std::cerr << label_path << " not exists" << std::endl;
            exit(-1);
        }
        std::vector<YoloLabelType> yoloLabels;
        int classId;
        float x, y, w, h;
        while (ifsm_label >> classId >> x >> y >> w >> h) {
            yoloLabels.push_back({x, y, w, h, (float) classId});
        }
        dataSet.push_back(std::make_pair(
                std::move(image_path), std::move(yoloLabels)));
    }
    ifsm.close();
    return std::move(dataSet);
}

SosoDataSet::SosoDataSet(const std::string &_train_list, const std::string &_test_list)
        : mode(Mode::kTrain) {
    trainSet = readYoloData(_train_list);
    testSet = readYoloData(_test_list);
}

torch::data::Example<> SosoDataSet::get(size_t index) {
    auto[img_path, labels]=mode == kTrain ? trainSet[index] : testSet[index];
    auto rawImg = cv::imread(img_path);
    int w = rawImg.cols, h = rawImg.rows;
    if (rawImg.empty()) {
        std::cerr << img_path << " is empty" << std::endl;
        exit(-1);
    }
    cv::cvtColor(rawImg, rawImg, cv::COLOR_BGR2RGB);
    auto[img, offsetParm]=padCvMatTo(rawImg, trainWidth, trainHeight);
    img.convertTo(rawImg, CV_32F, 1.0 / 255);
    std::swap(img, rawImg);
    auto imgTensor = torch::from_blob(
            img.data, {trainWidth, trainHeight, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous();
    std::vector<torch::Tensor> labelsTensorVec;
    const auto&[k, offsetW, offsetH]=offsetParm;
    for (const auto &label:labels) {
        auto newLabel = label;
        float kw = k * w;
        float kh = k * h;
        newLabel[0] = newLabel[0] * kw + offsetW;//x
        newLabel[1] = newLabel[1] * kh + offsetH;//y
        newLabel[2] *= kw;//w
        newLabel[3] *= kh;//h
        labelsTensorVec.push_back(
                torch::from_blob(newLabel.data(), {1, 5}, torch::kFloat32));
    }
    return {std::move(imgTensor), torch::cat(labelsTensorVec)};
}

torch::optional<size_t> SosoDataSet::size() const {
    return mode == kTrain ? trainSet.size() : testSet.size();
}

bool SosoDataSet::isTrain() const noexcept {
    return mode == kTrain;
}

void SosoDataSet::setTrainSize(int trainWidth, int trainHeight) {
    SosoDataSet::trainWidth = trainWidth;
    SosoDataSet::trainHeight = trainHeight;
}
