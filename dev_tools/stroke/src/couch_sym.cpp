#include "stroke/couch_sym.h"
#include "base/std_util.h"
#include "couch_sym_label.h" // const std::map<std::string, int> sStr2IntMap
#include <map>
#include <fstream>

static const std::string COUCH_SYM_PATH = TEST_SAMPLES_PATH + std::string("/datasets/couch-sym.dat");
static const std::vector<int> sLabelCharLikeLine = {
        8, 24, 53,
        130, 134, 141, 146, 158,
        172
};
static const std::vector<int> sLabelCharLikeCircle = {9, 21, 47};

HwScript HwDataLoader::GetByIntLabel(int label) {
    if (label >= mData.size()) {
        std::cerr << "label " << label << " out of boundary: [0," <<
                  mData.size() << ")" << std::endl;
        exit(-1);
    }
    auto &classData = mData[label];
    size_t idx = StdUtil::randInt() % classData.size();
    if (label == 43) {
        // 补丁：这个字符有问题
        while (idx == 43) idx = StdUtil::randInt() % classData.size();
    }
    return classData[idx].toHwScript();
}

HwScript HwDataLoader::GetByStrLabel(const std::string &label) {
    auto resItr = sStr2IntMap.find(label);
    if (resItr == sStr2IntMap.end()) {
        std::cerr << "fail to find " << label
                  << " in HWChar::sStr2IntMap" << std::endl;
        exit(-1);
    }
    return GetByIntLabel(resItr->second);
}

HwScript HwDataLoader::GetShape(const ShapeType &_shapeType) {
    switch (_shapeType) {
        case ShapeType::Line: {
            return GetByIntLabel(
                    sLabelCharLikeLine[StdUtil::randInt() % sLabelCharLikeLine.size()]);
        }
        case ShapeType::Circle: {
            auto circle = GetByIntLabel(
                    sLabelCharLikeCircle[StdUtil::randInt() % sLabelCharLikeCircle.size()]);
            circle.castToCircle(point2f(50, 50), 50, 50);
            return circle;
        }
        default: {
            std::cerr << (int) _shapeType << " not supported! "
                      << "Only \"line\" and \"circle\" supported!" << std::endl;
            exit(-1);
        }
    }
}

void HwDataLoader::LoadCouchDataSet() {
    mData.clear();
    std::ifstream ism(COUCH_SYM_PATH, std::ios::in | std::ios::binary);
    if (!ism.is_open()) {
        std::cerr << "fail to load " << COUCH_SYM_PATH << " with ifstream" << std::endl;
        exit(-1);
    }
    int a, b, c, d, e, iii = 0;
    // 【标签索引】、【宽度】、【长度】、【笔画数目】
    while (ism.read(reinterpret_cast<char *>(&a), 4)) {
        if (++iii % 10000 == 0) {
            std::cout << iii << std::endl;
        }
        ism.read(reinterpret_cast<char *>(&b), 4);
        ism.read(reinterpret_cast<char *>(&c), 4);
        ism.read(reinterpret_cast<char *>(&d), 4);
        std::vector<std::vector<point2f>> script;
        script.resize(d);
        for (int i = 0; i < d; i++) {
            // 【点数】
            ism.read(reinterpret_cast<char *>(&e), 4);
            script[i].resize(e);
            for (int j = 0; j < e; j++) {
                ism.read(reinterpret_cast<char *>(&script[i][j].first), 4);
                ism.read(reinterpret_cast<char *>(&script[i][j].second), 4);
            }
        }
        if (a >= mData.size()) {
            mData.resize(a + 1);
        }
        mData[a].push_back(std::move(script));
    }
    ism.close();
    for (auto &dat: mData) {
        dat.resize(dat.size());
    }
}

HwScript HwDataLoader::GetHwScript(const size_t &_classIndex, const size_t &_sampleIndex) {
    auto &sample = mData[_classIndex][_sampleIndex];
    return sample.toHwScript();
}

HwDataLoader::HwDataLoader() : isDataLoaded(false) {
}

HwDataLoader &HwDataLoader::getInstance() {
    static HwDataLoader e;
    if (!e.isDataLoaded) {
        e.LoadCouchDataSet();
        e.isDataLoaded = true;
    }
    return e;
}

HwDataSample::HwDataSample(std::vector<std::vector<point2f>> &&_data) {
    mData = _data;
}

HwScript HwDataSample::toHwScript() const {
    HwScript hwScript;
    for (auto &stroke: mData) {
        if (stroke.empty())continue;
        HwStroke hwStroke;
        for (auto &pt: stroke) {
            hwStroke.push_back(pt);
        }
        hwScript.push_back(hwStroke);
    }
    return hwScript;
}