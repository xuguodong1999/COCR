#include "ioutil.h"

namespace cocr {
    ErrorCode releaseToDir(const char *from, const char *to) {
        QDir fromDir(from);
        if (!fromDir.exists()) { return ErrorCode::InputDirMissing; }
        QDir toDir(to);
        if (!toDir.exists() && !toDir.mkpath(to)) {
            return ErrorCode::OutputDirMissing;
        }
        QFileInfoList fromDirFileInfoList = fromDir.entryInfoList();
        while (!fromDirFileInfoList.isEmpty()) {
            QFileInfo fileInfo = fromDirFileInfoList.last();
            if (fileInfo.isFile()) {
                const QString newFilePath = QString(to) + QDir::separator() + fileInfo.fileName();
                if (!QFile::exists(newFilePath) &&
                    !QFile::copy(fileInfo.absoluteFilePath(), newFilePath)) {
                    return ErrorCode::MoveFileFailed;
                }
                fromDirFileInfoList.removeLast();
            } else if (fileInfo.isDir()) {
                if (fileInfo.fileName() != "." && fileInfo.fileName() != "..") {
                    QDir subDir(fileInfo.filePath());
                    fromDirFileInfoList.removeLast();
                    fromDirFileInfoList.append(subDir.entryInfoList());
                }
            } else {
                fromDirFileInfoList.removeLast();
            }
        }
        return ErrorCode::NormalExec;
    }
}