#include "couchdataset.hpp"
#include <fstream>
#include <filesystem>
#include <random>
#include <opencv2/opencv.hpp>

void CouchDataSet::initMap(const char *_main_cast_txt, const char *_target_class) {
    std::ifstream ifsm(_main_cast_txt);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << _main_cast_txt << std::endl;
        exit(-1);
    }
    std::string labelContent;
    int labelIndex;
    std::map<std::string, int> sMainTable;
    while (ifsm >> labelContent >> labelIndex) {
        if (labelContent.empty()) {
            continue;
        }
        auto[_, success] = sMainTable.insert(std::make_pair(labelContent, labelIndex));
        if (!success) {
            std::cerr << "found " << labelContent << " twice. invalid cast table" << std::endl;
            exit(-1);
        }
    }
    ifsm.close();
    ifsm.open(_target_class);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << _target_class << std::endl;
        exit(-1);
    }
    std::set<int> numCountSet;
    while (ifsm >> labelContent >> labelIndex) {
        if (labelContent.empty()) {
            continue;
        }
        auto it = sMainTable.find(labelContent);
        if (it == sMainTable.end()) {
            std::cerr << labelContent << " not found in main table" << std::endl;
            exit(-1);
        }
        sTargetTable[it->first] = it->second;
        numCountSet.insert(it->second);
    }
    ifsm.close();
    sGlobal2GridVec.clear();
    for (auto&[content, index]:sTargetTable) {
        auto& dat = CouchLoader::GetData();
        size_t sampleNum = dat[index].size();
        for (size_t i = 0; i < sampleNum; i++) {
            sGlobal2GridVec.push_back(std::make_pair(index, i));
        }
    }
    sNumOfClass = numCountSet.size();
    size_t i=0;
    for(auto&index:numCountSet){
        sCastTable[index]=i++;
    }
    std::cout << "numOfClass=" << sNumOfClass << std::endl;
}

CouchDataSet::CouchDataSet() : mode(kTrain) {
    const char *all_class = "C:\\Users\\xgd\\source\\COCR\\data\\couch\\couch-gbk.txt";
    const char *target_class = "C:\\Users\\xgd\\source\\COCR\\data\\couch\\couch-gbk-target.txt";
    if (!isConfigLoaded) {
        CouchLoader::LoadCouchDataSet();
        initMap(all_class, target_class);
        isConfigLoaded = true;
    } else {
//        std::cerr<<"Config for Classifier has been loaded"<<std::endl;
//        exit(-1);
    }
    std::cout<<"done CouchDataSet::CouchDataSet()"<<std::endl;
}

void CouchDataSet::setTrainSize(int trainWidth, int trainHeight) {
    batchWidth = trainWidth;
    batchHeight = trainHeight;
}

torch::data::Example<> CouchDataSet::get(size_t index) {
    auto&[a, b]=sGlobal2GridVec[index];
    auto item = CouchLoader::GetShapeItem(a, b);
    cv::Mat img = cv::Mat(batchHeight, batchWidth, CV_8UC3, cvWhite);
    item.rotate(15-rand()%30);
    item.moveCenterTo(cv::Point2f(batchWidth / 2, batchHeight / 2));
    item.resizeTo(batchWidth * 0.8, batchHeight * 0.8, true);
    item.paintTo(img);
    img.convertTo(img, CV_32F, 1.0 / 255);
//    cv::imshow("CouchDataSet::get",img);
//    cv::waitKey(0);
    auto imgTensor = torch::from_blob(
            img.data, {batchWidth, batchHeight, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous();
//    auto target = torch::zeros(numOfClass, torch::kFloat32);
//    target[item.mLabel-1]=1;
    auto target = torch::tensor(sCastTable[item.mLabel]).to(torch::kInt64);
    return {std::move(imgTensor), std::move(target)};
}

torch::optional<size_t> CouchDataSet::size() const {
    return sGlobal2GridVec.size();
}

bool CouchDataSet::isTrain() const noexcept {
    return mode == kTrain;
}

void CouchDataSet::shuffle() {
    std::shuffle(sGlobal2GridVec.begin(),sGlobal2GridVec.end(),
                 std::default_random_engine());
}
