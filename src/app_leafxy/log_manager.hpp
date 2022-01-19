#ifndef _XGD_LOG_MANAGER_HPP_
#define _XGD_LOG_MANAGER_HPP_

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>

/**
 * @brief 简易的日志管理类，作为单例
 * @details
 * 初始化时调用initManager重定向
 * @note
 * 改为手动调用initManager是为了便于流程控制
 * 此外，也可以手动调用freeManager
 */
class LogManager : public QObject {
Q_OBJECT
    //保留默认handle，用于输出到控制台
    QtMessageHandler mMessageHandler;
    //输出到文件
    QFile mFile;
    //输出路径
    QString mFilePath;
    //多线程操作时需要加锁
    QMutex mMutex;
public:
    void writeLog(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static LogManager *GetInstance();

    void init(const QString &_dir);

private:
    LogManager();

    ~LogManager();

    void free();

    QString getLogFileName() const;
};

#endif//_XGD_LOG_MANAGER_HPP_
