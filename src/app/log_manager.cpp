#include "log_manager.hpp"

#include <QDir>
#include <QThread>
#include <QDateTime>
#include <QTextStream>
#include <QApplication>

static void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    LogManager::GetInstance()->writeLog(type, context, msg);
}

LogManager::LogManager() : mMessageHandler(nullptr) {
}

LogManager::~LogManager() {
    free();
}

LogManager *LogManager::GetInstance() {
    static LogManager instance;
    return &instance;
}

void LogManager::writeLog(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QMutexLocker locker(&mMutex);
    QString outBuffer;
    QTextStream stream(&outBuffer);
    switch (type) {
        case QtDebugMsg:
            stream << "[Debug]";
            break;
        case QtInfoMsg:
            stream << "[Info]";
            break;
        case QtWarningMsg:
            stream << "[Warning]";
            break;
        case QtCriticalMsg:
            stream << "[Critical]";
            break;
        case QtFatalMsg:
            stream << "[Fatal]";
            break;
        default:
            stream << "[Unknown]";
            break;
    }

    stream << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]")
           << QString("[%1]\t").arg((size_t) QThread::currentThreadId())
           << msg;

    // 写入文件
    if (mFile.isOpen()) {
        stream.setDevice(&mFile);
        stream << outBuffer << "\n";
    }

    // 默认的输出，控制台
    QString shellBuffer;
    stream.setString(&shellBuffer);
    stream << outBuffer;
    mMessageHandler(type, context, shellBuffer);
}

void LogManager::init(const QString &_dir) {
    mFilePath = _dir;
    QDir dir(mFilePath);
    if (!dir.exists()) {
        dir.mkpath(mFilePath);
        if (!dir.exists()) {
            throw std::runtime_error(std::string("error path:") + _dir.toStdString());
        }
    }
    free();
    mFile.setFileName(mFilePath + QDir::separator() + getLogFileName());
    if (!mFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        throw std::runtime_error(std::string("error file:") + mFile.fileName().toStdString());
    }
    mMessageHandler = qInstallMessageHandler(handleMessage);
}

void LogManager::free() {
    if (mFile.isOpen()) { mFile.close(); }
    qInstallMessageHandler(nullptr);
}

QString LogManager::getLogFileName() const {
    return QString("leafxy_log@%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"));
}
