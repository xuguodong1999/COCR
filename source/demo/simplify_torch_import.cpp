#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <functional>
#include <iostream>
#include <optional>

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

std::vector<std::pair<QString, QString>> MM_MAP = {
        {"#include <torch/nn/modules/linear.h>",               "nn::Linear"},
        {"#include <torch/nn/modules/conv.h>",                 "nn::Conv2d"},
        {"#include <torch/nn/modules/batchnorm.h>",            "nn::BatchNorm"},
        {"#include <torch/optim/adam.h>",                      "optim::Adam"},
        {"#include <torch/optim/sgd.h>",                       "optim::SGD"},
        {"#include <torch/nn/modules/normalization.h>",        "nn::LocalResponseNorm"},
        {"#include <torch/serialize.h>",                       "torch::save"},
        {"#include <torch/serialize.h>",                       "torch::load"},
        {"#include <torch/nn/modules/activation.h>",           "nn::LeakyReLU"},
        {"#include <torch/nn/modules/embedding.h>",            "nn::Embedding"},
        {"#include <torch/nn/modules/rnn.h>",                  "nn::LSTM"},
        {"#include <torch/nn/modules/activation.h>",           "nn::ReLU"},
        {"#include <torch/nn/modules/activation.h>",           "nn::Tanh"},
        {"#include <torch/cuda.h>",                            "torch::cuda"},
        {"#include <torch/nn/modules/container/sequential.h>", "nn::Sequential"},
        {"#include <torch/nn/modules/loss.h>",                 "nn::L1Loss"},
        {"#include <torch/nn/modules/loss.h>",                 "nn::L2Loss"},
        {"#include <torch/nn/module.h>",                       "nn::Module"},
        {"#include <torch/nn/init.h>",                         "nn::init"},
        {"#include <torch/nn/modules/conv.h>",                 "F::conv2d"},
        {"#include <torch/nn/modules/pooling.h>",              "nn::MaxPool2d"},
        {"#include <torch/nn/options/upsampling.h>",           "nn::UpsampleOptions"},
        {"#include <torch/nn/functional/loss.h>",              "functional::cross_entropy"},
        {"#include <torch/nn/modules/pooling.h>",              "nn::AdaptiveAvgPool2d"},
        {"#include <torch/data/transforms.h>",                 "data::transforms"},
        {"#include <torch/data/dataloader.h>",                 "data::make_data_loader"},
        {"#include <torch/data/datasets.h>",                   "data::datasets"},
        {"#include <torch/nn/modules/dropout.h>",              "nn::Dropout"},
        {"#include <torch/nn/options/upsampling.h>",           "data::nn::UpsampleOptions"},
        {"#include <torch/nn/functional/upsampling.h>",        "F::interpolate"}
};
const char *NN_TYPES_H = "#include <torch/types.h>";


std::optional<QString> processSourceFile(const QString &codes) {
    QString result = codes;
    auto torchBegin = result.indexOf(NN_TYPES_H);
    if (-1 == torchBegin) return std::nullopt;
    auto insertPos = result.indexOf("\n", torchBegin);
    if (-1 == insertPos) return std::nullopt;
    for (auto&[header, feature]: MM_MAP) {
        if (result.contains(feature) && !result.contains(header)) {
            result.insert(insertPos, "\n" + header);
        }
    }
    return result;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QDir dir(QDir::home());
    if (!dir.cd("./source/repos/dev/tutorials/libtorch")) {
        return EXIT_FAILURE;
    }
    std::vector<QString> files;
    diveInDir(dir, [&](const QString &path) -> void {
        files.push_back(path);
    }, false);
    for (auto &path: files) {
        qDebug() << path;
        QFile f(path);
        if (f.open(QIODevice::ReadWrite)) {
            const auto buffer = f.readAll();
            f.close();
            auto ret = processSourceFile(buffer);
            if (!ret) {
                continue;
            }
            const auto result = ret.value().toLocal8Bit();
            if (f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
                f.write(result);
                f.close();
            }
        }
    }
    return EXIT_SUCCESS;
}