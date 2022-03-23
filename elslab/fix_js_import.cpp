#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <functional>
#include <iostream>

template<typename T>
using f = const std::function<T>;

void doSomething(const QString &filename) {
    QFile f(QDir::home().absolutePath() + "/workspace/f1/" + filename);
    if (f.open(QIODevice::ReadOnly)) {
        bool startRevert = false, allowIn = false;
        QString result;
        do {
            QString lineResult;
            auto line = f.readLine();
            if (line.startsWith("<<<<<<<")) { // HEAD Current change
                startRevert = true;
                allowIn = false;
                continue;
            } else if (line.startsWith(">>>>>>>")) { // Incoming change
                startRevert = false;
                allowIn = false;
                continue;
            } else if (line.startsWith("=======")) { // split
                allowIn = true;
                continue;
            }
            if (!startRevert || allowIn) {
                result.append(lineResult);
            }
        } while (!f.atEnd());
        f.close();
//        if (f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
//            f.write(result.toLocal8Bit());
//            f.close();
//        }
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QFile fileListFile(QDir::home().absolutePath() + "/Desktop/TEMP.txt");
    if (!fileListFile.open(QIODevice::ReadOnly)) {
        return -2;
    }
    do {
        auto buffer = fileListFile.readLine();
        qDebug() << buffer;
        doSomething(buffer.trimmed());
    } while (!fileListFile.atEnd());
    return EXIT_SUCCESS;
}