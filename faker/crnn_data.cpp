#include "crnn_data.hpp"
#include "hw_str.hpp"
#include "linetextdata.hpp"
#include "colors.hpp"
#include "std_util.hpp"

#include <lmdb++.h>

#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

CRNNDataGenerator::CRNNDataGenerator() : isInited(false) {
}

bool CRNNDataGenerator::init(const char *_topDir) {
    topDir = _topDir;
    if (!std::filesystem::exists(topDir)) {
        if (!std::filesystem::create_directories(topDir)) {
            std::cerr << "fail to create dir: " << topDir << std::endl;
            exit(-1);
            return false;
        }
    }
    isInited = true;
    return true;
}

static std::vector<HwController> controllers = {
        HwController(3),
        HwController(4),
        HwController(5),
        HwController(6),
        HwController(7),
        HwController(8),
};

void CRNNDataGenerator::dump() {
    if (!isInited) {
        std::cerr << "you must call CRNNDataGenerator::init before dump data" << std::endl;
        exit(-1);
    }
    auto env = lmdb::env::create();
    env.set_mapsize(1UL * 1024UL * 1024UL * 1024UL * 1024UL); // 1 TB
    auto dbPath = topDir + "/" + dbName;
    if (std::filesystem::exists(dbPath)) {
        std::filesystem::remove(dbPath);
        if (std::filesystem::exists(dbPath)) {
            std::cerr << "fail to delete file " << dbPath << std::endl;
            exit(-1);
        }
    }
    std::cout << "path=" << dbPath << std::endl;
    env.open(topDir.c_str(), 0, 0664);
    auto wtxn = lmdb::txn::begin(env);
    auto dbi = lmdb::dbi::open(wtxn, nullptr);
    LineTextDataCreator lineTextDataCreator;
//    lineTextDataCreator.loadFromPattern();
    lineTextDataCreator.loadFromWordDict();
    size_t idx = 0;
    lineTextDataCreator.loopOnce([&](const std::string &_text) {
        HwStr hwStr;
        hwStr.loadRichACSII(_text);
        float k = betweenProb(1.3, 2.2);
        hwStr.resizeTo(width * k - 4, height * k - 4, true);
        auto rect = hwStr.getBoundingBox().value();
        if (rect.width > width * k - 4) {
            hwStr.resizeTo(width * k - 4, height * k - 4, false);
        }
        cv::Mat img = cv::Mat(height * k, width * k, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        HwController hwController(2);
        hwStr.setHwController(hwController);
        hwStr.moveLeftTopTo(cv::Point2f(1, 1));
        hwStr.paintTo(img);
        cv::resize(img, img, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
        if (byProb(0.5)) {// 反转颜色
            cv::bitwise_not(img, img);
        }
        std::vector<uchar> buffer;
        cv::imencode(".jpg", img, buffer, {cv::IMWRITE_JPEG_QUALITY, 100});
        buffer.push_back('\0');
        std::cout << "********************\nlabel=" << _text << std::endl;
        char a[100];
        sprintf(a, imgKey, idx);
        std::cout << "image-key=" << a << std::endl;
        dbi.put(wtxn, a, buffer.data());
        sprintf(a, labelKey, idx++);
        std::cout << "label-key=" << a << std::endl;
        dbi.put(wtxn, a, _text.c_str());
//        wtxn.commit();
        if (idx > 30000) {
            wtxn.commit();
            exit(-1);
        }
    });
    wtxn.commit();
}

void CRNNDataGenerator::display() {
    LineTextDataCreator lineTextDataCreator;
//    lineTextDataCreator.loadFromPattern();
    lineTextDataCreator.loadFromWordDict();
//    lineTextDataCreator.loadFromSuperAtom();

    size_t idx = 0;
    lineTextDataCreator.loopOnce([&](const std::string &_text) {
        HwStr hwStr;
        hwStr.loadRichACSII(_text);
        float k = betweenProb(1.3, 2.2);
        hwStr.resizeTo(width * k - 4, height * k - 4, true);
        auto rect = hwStr.getBoundingBox().value();
        if (rect.width > width * k - 4) {
            hwStr.resizeTo(width * k - 4, height * k - 4, false);
        }
        cv::Mat img = cv::Mat(height * k, width * k, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        HwController hwController(2);
        hwStr.setHwController(hwController);
        hwStr.moveLeftTopTo(cv::Point2f(1, 1));
//        hwStr.paintTo(img);
        cv::putText(img, _text, cv::Point(0,height * k/1.4),
                    2, 1.5, getScalar(ColorName::rgbBlack),
                    1, cv::LINE_AA,false);
        cv::resize(img, img, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
        if (byProb(0.5)) {// 反转颜色
            cv::bitwise_not(img, img);
        }
        std::vector<uchar> buffer;
        cv::imencode(".jpg", img, buffer, {cv::IMWRITE_JPEG_QUALITY, 100});
        buffer.push_back('\0');
        std::cout << "********************\nlabel=" << _text << std::endl;
        char a[100];
        sprintf(a, imgKey, idx);
        std::cout << "image-key=" << a << std::endl;
        sprintf(a, labelKey, idx++);
        std::cout << "label-key=" << a << std::endl;


        auto img2 = cv::imdecode(buffer, CV_8UC1);
        cv::imshow("origin-data", img);
        cv::imshow("ed-data", img2);
        cv::waitKey(0);
    });
}
