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
}

MemDataGenerator::MemDataGenerator()
        : sampleNum(256), imgWidth(416), imgHeight(416), imgChannel(3), labelSize(10240 * sizeof(float)),
          sleepTime(1000), stop(false), idx(0) {
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
        mem.lock();
        uchar *beg = (uchar *) mem.data() + idx * sampleSize;
        memcpy(beg, img.data, imgSize);
        beg += imgSize;
//        memcpy(beg, labels.data(), labelSize);
        mem.unlock();
        cv::Mat img2;
        img2.flags = CV_32FC3;
        img2.rows = imgHeight;
        img2.cols = imgWidth;
        img2.channels();
        img2.data = (uchar *) mem.data() + idx * sampleSize;
        cv::imshow(memKey, img2);
        idx = (idx + 1) % sampleNum;
        qDebug() << "loop...";
        QThread::msleep(sleepTime);
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
