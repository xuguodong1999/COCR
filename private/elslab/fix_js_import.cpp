#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <functional>
#include <iostream>

template<typename T>
using f = const std::function<T>;
QSet<QString> allComponents;
const QString err_import = "import {ErrorCode} from '@xxx/zzz/error-code';";

void doSomething(const QString &filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        QString fileContent;
        QSet<QString> components;
        bool needErrorCode = false;
        bool needRewrite = false;
        do {
            auto line = f.readLine();
            if (line.startsWith("import") && line.contains("@xxx/yyy") && !line.contains("/img/")) {
                auto importBeg = line.indexOf("import") + 6;
                auto fromBeg = line.indexOf("from");
                auto target = line.mid(importBeg, fromBeg - importBeg);
                target.replace('{', ' ');
                target.replace('}', ' ');
                target.replace(',', ' ');
                auto words = target.split(' ');
                for (const auto &word: words) {
                    if (word == "XXX") {
                        needErrorCode = true;
                    } else if (!word.isEmpty()) {
                        components.insert(word);
                    }
                }
            } else {
                fileContent.append(line);
            }
        } while (!f.atEnd());
        f.close();
        if (!components.empty()) {
            needRewrite = true;
            qDebug() << components;
            allComponents.unite(components);
            QString yyy_import = "import {";
            for (const auto &component: components) {
                yyy_import += component;
                yyy_import += ", ";
            }
            yyy_import += "} from '@xxx/yyy';";
            fileContent = yyy_import + "\n" + fileContent;
        }
        if (needErrorCode) {
            needRewrite = true;
            fileContent = err_import + "\n" + fileContent;
//            qDebug() << fileContent.toStdString().c_str();
//            std::cin.get();
        }
        if (!needRewrite) { return; }
        if (f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
            f.write(fileContent.toLocal8Bit());
            f.close();
        }
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
//        qDebug() << buffer;
        doSomething(buffer.trimmed());
    } while (!fileListFile.atEnd());
    qDebug() << allComponents;
    return EXIT_SUCCESS;
}