#include "soso_mem.hpp"
#include "colors.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
#include <QThread>
#include <iostream>

const std::string &MemDataGenerator::getMemKey() const {
    return memKey;
}

void MemDataGenerator::setMemKey(const std::string &memKey) {
    MemDataGenerator::memKey = memKey;
}

void MemDataGenerator::reAllocateMem() {
    mem.setKey(memKey.c_str());
    qDebug() << "create " << memSize / 1024 / 1024 << "MB shared memory";
    if (!mem.create(memSize, QSharedMemory::ReadWrite)) {
        qDebug() << mem.errorString();
        mem.attach();
        std::cerr << "Try run again." << std::endl;
    } else {
        qDebug() << "isAttached = " << mem.isAttached();
    }
    for (auto &lock:locks) {
        if (lock) {
            delete lock;
            lock = nullptr;
        }
    }
    locks.clear();
    for (size_t i = 0; i < sampleNum; i++) {
        auto lock1 = new QSharedMemory(("q" + std::to_string(i) + ".jpg").c_str());
        auto lock2 = new QSharedMemory(("q" + std::to_string(i) + ".txt").c_str());
        if (!lock1->create(1, QSharedMemory::ReadOnly)) {
            qDebug() << lock1->errorString();
            lock1->attach();
//            std::cerr << "Try run again." << std::endl;
        }
        if (!lock2->create(1, QSharedMemory::ReadOnly)) {
            qDebug() << lock2->errorString();
            lock2->attach();
//            std::cerr << "Try run again." << std::endl;
        }
        locks.push_back(lock1);
        locks.push_back(lock2);
    }
}

MemDataGenerator::MemDataGenerator()
        : sampleNum(256), imgWidth(416), imgHeight(416), imgChannel(3), labelSize(10240 * sizeof(float)),
          sleepTime(100), stop(false), idx(0) {
    updateParm();
}

void MemDataGenerator::set(const size_t &_sampleNum, const size_t &_imgWidth, const size_t &_imgHeight,
                           const size_t &_imgChannel, const size_t &_labelSize) {
    sampleNum = _sampleNum;
    imgWidth = _imgWidth;
    imgHeight = _imgHeight;
    imgChannel = _imgChannel;
    labelSize = _labelSize;
    updateParm();
    reAllocateMem();
}

void MemDataGenerator::run() {
    while (!isStop()) {
        cv::Mat img(imgHeight, imgWidth, CV_32FC3,
                    getScalar(ColorName::rgbBlue));
        std::vector<float> labels;
        // get data above
        labels.resize(labelSize / sizeof(float), 5);

        locks[2 * idx]->lock();
        locks[2 * idx + 1]->lock();
        uchar *beg = (uchar *) mem.data() + idx * sampleSize;
        memcpy(beg, img.data, imgSize);
        beg += imgSize;
//        memcpy(beg, labels.data(), labelSize);
        locks[2 * idx]->unlock();
        locks[2 * idx + 1]->unlock();
        cv::Mat img2;
        img2.flags = CV_32FC3;
        img2.rows = imgHeight;
        img2.cols = imgWidth;
        img2.channels();
        img2.data = (uchar *) mem.data() + idx * sampleSize;
        cv::imshow(memKey, img2);
        idx = (idx + 1) % sampleNum;
        qDebug() << "loop...";
        count++;
        if (count >= sampleNum) {
            QThread::msleep(sleepTime);
        }
    }
    qDebug() << "exit";
}

void MemDataGenerator::updateParm() {
    imgSize = sizeof(float) * imgChannel * imgHeight * imgWidth;
    sampleSize = imgSize + labelSize;
    memSize = sampleSize * sampleNum;
}

bool MemDataGenerator::isStop() const {
    return stop;
}


size_t MemDataGenerator::getSleepTime() const {
    return sleepTime;
}

void MemDataGenerator::setSleepTime(size_t sleepTime) {
    MemDataGenerator::sleepTime = sleepTime;
}

void MemDataGenerator::exit_run() {
    stop = true;
}

MemDataGenerator::~MemDataGenerator() {
    qDebug() << "fuck:" << __FUNCTION__;
    for (auto &lock:locks) {
        if (lock) {
            delete lock;
            lock = nullptr;
        }
    }
}

MCWidget::MCWidget(QWidget *_parent) : QWidget(_parent) {
    dg = new MemDataGenerator();
    dg->setMemKey("fuck");
    dg->set(512, 416, 416, 3, sizeof(float) * 10240);
    dg->start();
//    connect(this,&MCWidget::sig_closed,&dg,&MemDataGenerator::exit_run);
}

void MCWidget::closeEvent(QCloseEvent *e) {
    emit sig_closed();
    dg->exit_run();
//    dg.quit();
    qDebug() << dg->wait();
    delete dg;
//    dg->deleteLater();
//    dg.quit();
//    QThread::msleep(2000);
    QWidget::closeEvent(e);
}



std::optional<cv::Mat> MemDataGenerator::readImageFromMem(const char *_filename) {
    if (jpgMap.empty()) {
        for (size_t i = 0; i < 10240; i++)jpgMap["q" + std::to_string(i) + ".jpg"] = i;
    }
    auto itr = jpgMap.find(_filename);
    if (itr == jpgMap.end()) {
        return std::nullopt;
    }
    locks[itr->second]->lock();
    cv::Mat img2;
    img2.flags = CV_32FC3;
    img2.rows = 416;
    img2.cols = 416;
    img2.channels();
    img2.data = (uchar *) mem.data() + idx * sampleSize;
    locks[itr->second]->unlock();
    return std::nullopt;
}

std::optional<std::vector<std::tuple<int, float, float, float, float, float>>>
        MemDataGenerator::readLabelFromMem(const char *_filename) {
    if (txtMap.empty()) {
        for (size_t i = 0; i < 10240; i++)txtMap["q" + std::to_string(i) + ".jpg"] = i;
    }
    auto itr = txtMap.find(_filename);
    if (itr == txtMap.end()) {
        return std::nullopt;
    }
    locks[itr->second]->lock();

    locks[itr->second]->unlock();
    return std::nullopt;
}