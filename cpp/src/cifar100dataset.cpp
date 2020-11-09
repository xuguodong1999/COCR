#include "cifar100dataset.hpp"
#include "opencvutil.hpp"

#include <opencv2/opencv.hpp>


#include <fstream>
#include <filesystem>
#include <random>


torch::data::Example<> Cifar100DataSet::get(size_t index) {}

torch::optional<size_t> Cifar100DataSet::size() const {

}

bool Cifar100DataSet::isTrainMode() const noexcept {
    return mode == kTrain;
}

size_t Cifar100DataSet::GetNumOfClass() {

}

Cifar100DataSet::Cifar100DataSet(const Cifar100DataSet::Mode &_mode) {

}

void Cifar100DataSet::setMode(bool _isTrain) noexcept {
    mode = _isTrain ? kTrain : kTest;
}

