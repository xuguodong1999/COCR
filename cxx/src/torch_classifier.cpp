#include "torch_classifier.hpp"
#include <torch/torch.h>
#include <filesystem>

using namespace torch;
using namespace torch::nn;

BaseClassifier::BaseClassifier(
        Sequential _layerIn,
        std::vector<Sequential> _bnecks,
        Sequential _layerOut)
        : name("classifier"),
          layerIn(std::move(_layerIn)),
          bnecks(std::move(_bnecks)),
          layerOut(std::move(_layerOut)) {
}

void BaseClassifier::saveClassifier(
        const std::string &_saveDir, const std::string &_inPrefix,
        const std::string &_bneckPrefix, const std::string &_outPrefix) {
    if (!std::filesystem::exists(_saveDir)) {
        if (!std::filesystem::create_directories(_saveDir)) {
            std::cerr << "BaseClassifier::saveClassifier: Have no access to "
                      << _saveDir << std::endl;
            exit(-1);
        }
    }
    const std::string prefix = _saveDir + "/" + name + "-";
    try {
        torch::save(layerIn, prefix + _inPrefix + ".pth");
        for (size_t i = 0; i < bnecks.size(); i++) {
            torch::save(bnecks[i], prefix + _bneckPrefix + "-d" +
                                   std::to_string(i + 1) + "x.pth");
        }
        torch::save(layerOut, prefix + _outPrefix + ".pth");
    } catch (std::exception &e) {
        std::cerr << "In BaseClassifier::saveClassifier" << std::endl;
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}

void BaseClassifier::loadClassifier(
        const std::string &_saveDir, const std::string &_inPrefix,
        const std::string &_bneckPrefix, const std::string &_outPrefix,
        const torch::Device &_device) {
    const std::string prefix = _saveDir + "/" + name + "-";
    try {
        if(!_inPrefix.empty()) {
            torch::load(layerIn, prefix + _inPrefix + ".pth", _device);
        }
        if(!_bneckPrefix.empty()) {
            for (size_t i = 0; i < bnecks.size(); i++) {
                torch::load(bnecks[i], prefix + _bneckPrefix + "-d" +
                                       std::to_string(i + 1) + "x.pth", _device);
            }
        }
        if(!_outPrefix.empty()) {
            torch::load(layerOut, prefix + _outPrefix + ".pth", _device);
        }
    } catch (std::exception &e) {
        std::cerr << "In BaseClassifier::loadClassifier" << std::endl;
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}

const std::string &BaseClassifier::getName() const {
    return name;
}

void BaseClassifier::setName(const std::string &name) {
    BaseClassifier::name = name;
}

void BaseClassifier::freezeInLayer(bool _freeze) {
    bool requireGrad = !_freeze;
    for (auto &parm:layerIn->parameters(true)) {
        parm.set_requires_grad(requireGrad);
    }
}

void BaseClassifier::freezeBneck(const size_t &i, bool _freeze) {
    if (i < 0 || i >= bnecks.size()) {
        std::cerr << "bneck index " << i << " out of range while bnecks.size()="
                  << bnecks.size() << std::endl;
        exit(-1);
    }
    bool requireGrad = !_freeze;
    for (auto &parm:bnecks[i]->parameters(true)) {
        parm.set_requires_grad(requireGrad);
    }
}

void BaseClassifier::freezeOutLayer(bool _freeze) {
    bool requireGrad = !_freeze;
    for (auto &parm:layerOut->parameters(true)) {
        parm.set_requires_grad(requireGrad);
    }
}

