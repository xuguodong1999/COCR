#ifndef _SOSO_QSM_HPP_
#define _SOSO_QSM_HPP_

#include <QSharedMemory>
#include <QRunnable>
#include <QList>
#include <memory>
#include <string>

/**
 * 维护大小固定的图像对应的数据和标签所在的共享内存池
 */
class SOSOMemory : public QRunnable {
    size_t IMG_NUM;// 样本数量
    size_t WIDTH, HEIGHT;// 3通道图像大小
    size_t LABEL_SIZE;// 为一个图像对应的标签预留的空间大小
    size_t SAMPLE_SIZE, MAX_SIZE, IMG_SIZE;
    QSharedMemory mem;
    std::unordered_map<std::string,size_t>keys;
//    std::vector<std::string> keys;
    std::string key;
    int sleepTime;
    size_t idx;
public:
    bool isStop() const;

    void setStop(bool _stop);

public:
    int getSleepTime() const;

    void setSleepTime(int sleepTime);

    explicit SOSOMemory(const char *_key,
                        const size_t &_IMG_NUM = 1024,
                        const size_t &_WIDTH = 416, const size_t &_HEIGHT = 416,
                        const size_t &_LABEL_SIZE = 10240 * sizeof(float));

    ~SOSOMemory();

    void run();
};

#endif//_SOSO_QSM_HPP_
