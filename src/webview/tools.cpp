#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <functional>
#include <iostream>
#include <QRandomGenerator>
#include <QMutex>

template<typename T>
using f = const std::function<T>;

void diveInDir(const QDir &dirName, f<void(const QString &)> &func, bool withDir = false) {
    QDir dir(dirName);
    if (!dir.exists()) return;
    QDir::Filters filter = QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot;
    dir.setFilter(filter);
    dir.setSorting(QDir::Name);
    QFileInfoList dirItems = dir.entryInfoList();
    while (!dirItems.isEmpty()) {
        QFileInfo lastItem = dirItems.last();
        dirItems.removeLast();
        if (!lastItem.isDir()) {
            func(lastItem.absoluteFilePath());
        } else {
            QDir subDir(lastItem.filePath());
            if (withDir) func(lastItem.absoluteFilePath());
            subDir.setFilter(filter);
            dirItems.append(subDir.entryInfoList());
        }
    }
}

QMap<QString, QString> include2PathMap;

void addHeadersByFile(const QString &filePath, QSet<QString> &dst) {
    QFile srcFile(filePath);
    if (!srcFile.open(QFile::ReadOnly)) {
        qDebug() << srcFile.error();
        return;
    }
    QString line = srcFile.readLine();
    const QRegExp headerReg(R"((<[^>]+>)|("[^"]+"))"), headerPrefixReg(R"((\s+)?#(\s+)?include)");
    while (!line.isEmpty()) {
        if (headerPrefixReg.indexIn(line, 0) >= 0) {
            for (int i = 0; i = headerReg.indexIn(line, i), i >= 0; i += headerReg.matchedLength()) {
                QString header = headerReg.cap(0);
                if (header.length() < 2) continue;
                QString trippedHeader = header.mid(1, header.length() - 2);
                if (include2PathMap.contains(trippedHeader)) {
                    dst.insert(trippedHeader);
                }
            }
        }
        line = srcFile.readLine();
    }
}

/**
 * 统计源码引用了哪些头文件
 * @param sourceDirList 源码路径
 * @param includeDirList 头文件搜索路径
 * @return
 */
int count(const QList<QString> &sourceDirList, const QList<QString> &includeDirList) {
    // 统计项目的头文件搜索路径
    if (include2PathMap.empty())
        for (const auto &includeDirPath: includeDirList) {
            const QDir includeDir(includeDirPath);
            diveInDir(includeDirPath, [&](const QString &filePath) -> void {
                include2PathMap.insert(includeDir.relativeFilePath(filePath), filePath);
            });
        }

    // 统计目标代码的头文件引用
    QSet<QString> sourceIncludeSet;
    for (const auto &srcDir: sourceDirList) {
        diveInDir(srcDir, [&](const QString &filePath) -> void {
            addHeadersByFile(filePath, sourceIncludeSet);
        });
    }

    // 迭代直到头文件数目稳定
    QSet<QString> fullIncludeSet(sourceIncludeSet), addOnIncludeSet(sourceIncludeSet);
    int stableNum = -1;
    while (fullIncludeSet.size() != stableNum) {
        stableNum = fullIncludeSet.size();
        QSet<QString> emptyAddOnIncludeSet;
        for (auto &header: addOnIncludeSet) {
            auto it = include2PathMap.find(header);
            if (it == include2PathMap.end()) continue;
            addHeadersByFile(it.value(), emptyAddOnIncludeSet);
        }
        addOnIncludeSet.clear();
        for (auto &header: emptyAddOnIncludeSet) {
            if (!fullIncludeSet.contains(header)) {
                fullIncludeSet.insert(header);
                addOnIncludeSet.insert(header);
            }
        }
    }

    // 检查
//    for (const auto &header: sourceIncludeSet) qDebug() << header;
//    auto it = include2PathMap.begin();
//    while (it != include2PathMap.end()) qDebug() << (it++).key();
//    for (auto &header: fullIncludeSet) {
//        if (header.startsWith("boost")) qDebug() << header;
//    }
    return fullIncludeSet.size();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QDir Boost_INCLUDE_DIR("/home/xgd/shared/boost/include");
    QDir moduleDir(Boost_INCLUDE_DIR.absolutePath() + "/boost");
    moduleDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QMap<int, QString> statistics;
    count({}, {Boost_INCLUDE_DIR.absolutePath()});
    auto infoList = moduleDir.entryInfoList();
    QMutex mutex;
    QMutexLocker locker(&mutex);
#pragma omp parallel for
    for (int i = 0; i < infoList.size(); i++) {
        const auto &module = infoList.at(i);
        int n = count({module.absoluteFilePath()}, {Boost_INCLUDE_DIR.absolutePath()});
        locker.relock();
        statistics.insert(n, Boost_INCLUDE_DIR.relativeFilePath(module.absoluteFilePath()));
        locker.unlock();
    }
    auto it = statistics.begin();
    std::string buffer;
    int i = 0;
    while (it != statistics.end()) {
        buffer += it.value().toStdString();
        buffer += ": ";
        buffer += std::to_string(it++.key());
        buffer += "\t";
        if (++i % 3 == 0) buffer += "\n";
    }
    std::cout << buffer << std::endl;
    return EXIT_SUCCESS;
}