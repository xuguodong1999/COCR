#include "soso_qsm.hpp"
#include "colors.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <QThread>
#include <QDebug>
#include <iostream>

SOSOMemory::SOSOMemory(const char *_key,
                       const size_t &_IMG_NUM, const size_t &_WIDTH,
                       const size_t &_HEIGHT, const size_t &_LABEL_SIZE)
        : IMG_NUM(_IMG_NUM), WIDTH(_WIDTH), HEIGHT(_HEIGHT),
          LABEL_SIZE(_LABEL_SIZE), key(_key), idx(0), sleepTime(2000) {
    if (key.empty()) {
        key = "cocr-data-memory";
    }
    IMG_SIZE = WIDTH * HEIGHT * 3 * sizeof(float);
    SAMPLE_SIZE = IMG_SIZE + LABEL_SIZE;
    MAX_SIZE = _IMG_NUM * SAMPLE_SIZE;


    for (size_t i = 0; i < IMG_NUM; i++) {
        keys[key + std::to_string(i)] = i;
    }
    qDebug() << "create " << MAX_SIZE / 1024 / 1024 << " MB";
    bool state = mem.create(MAX_SIZE, QSharedMemory::ReadWrite);
    if (!state && QSharedMemory::AlreadyExists == mem.error()) {
        std::cerr << "QSharedMemory::AlreadyExists" << std::endl;
        mem.attach();// 接管并试图正常析构
    }
}

int SOSOMemory::getSleepTime() const {
    return sleepTime;
}

void SOSOMemory::setSleepTime(int sleepTime) {
    SOSOMemory::sleepTime = sleepTime;
}

#include <QAtomicInt>

static QAtomicInt stop;

void SOSOMemory::run() {
    stop = 0;
    while (true) {
        cv::Mat img(416, 416, CV_32FC3,
                    getScalar(ColorName::rgbBlue));
        std::vector<float> labels;
        labels.resize(100, 5);
//        float *beg = (float *) qsmVec[idx]->data() + idx * SAMPLE_SIZE;
//        qsmVec[idx]->lock();
//        memcpy(beg, img.data, IMG_SIZE);
//        beg += IMG_SIZE;
//        memcpy(beg, labels.data(), labels.size() * sizeof(float));
//        qsmVec[idx]->unlock();
        idx = (idx + 1) % IMG_NUM;
        qDebug() << sleepTime << "ms";
        qDebug() << "stop=" << stop;
        QThread::msleep(sleepTime);
        if (isStop())
            break;
    }
}

bool SOSOMemory::isStop() const {
    return stop;
}

void SOSOMemory::setStop(bool _stop) {
    stop = _stop;
    std::cout << "stop====" << stop << std::endl;
    qDebug() << stop;
}

SOSOMemory::~SOSOMemory() {
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
}
