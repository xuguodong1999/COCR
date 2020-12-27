#include "openbabel_util.hpp"

#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/plugin.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

static std::unordered_map<size_t, size_t> j2oAidMap;
static std::unordered_map<size_t, size_t> o2jAidMap;

OpenBabel::OBMol convertJMolToOBMol(const std::shared_ptr<JMol> &_jMol) {
    OpenBabel::OBMol obMol;
    // TODO: 转换
    return std::move(obMol);
}

std::shared_ptr<JMol> convertOBMolToJMol(const OpenBabel::OBMol &_obMol) {
    auto jMol = std::make_shared<JMol>();
    // TODO: 转换
    return jMol;
}

bool runOBForceField(OpenBabel::OBMol &_obMol, const std::string &_forcefield) {
    using namespace OpenBabel;
    if (_obMol.Empty()) {
        std::cerr << "mol.Empty()" << std::endl;
        return false;
    }
    auto pFF = static_cast<OBForceField *>(OBPlugin::GetPlugin(
            "forcefields", _forcefield.c_str()));
    if (!pFF) {
        std::cerr << "runOBForceField: could not find forcefield '"
                  << _forcefield << "'." << std::endl;
        return false;
    }
    pFF->SetLogFile(&std::cerr);
    pFF->SetLogLevel(OBFF_LOGLVL_MEDIUM);
    try {
        OBBuilder builder;
        builder.Build(_obMol);
        _obMol.AddHydrogens(false, true);
    } catch (std::exception &e) {
        std::cerr << "runOBForceField build & add H step:" << e.what() << std::endl;
        return false;
    }
    if (!pFF->Setup(_obMol)) {
        std::cerr << "runOBForceField: could not setup force field." << std::endl;
        return false;
    }
    try {
        pFF->SteepestDescent(500, 1.0e-4);
        pFF->WeightedRotorSearch(250, 50);
        pFF->SteepestDescent(500, 1.0e-6);
        pFF->UpdateCoordinates(_obMol);
    } catch (std::exception &e) {
        std::cerr << "runOBForceField MD step:" << e.what() << std::endl;
        return false;
    }
    return true;
}


std::string MolUtilOpenBabelImpl::getSMILES(const std::shared_ptr<JMol> &_mol) {
    // can -- Canonical SMILES format
    return getFormat(_mol, "can");
}

std::vector<std::string> MolUtilOpenBabelImpl::getForcefieldChoices() {
    std::vector<std::string> result;
    OpenBabel::OBPlugin::ListAsVector("forcefields", nullptr, result);
    return std::move(result);
}

std::vector<std::string> MolUtilOpenBabelImpl::getFormatChoices() {
    std::vector<std::string> result;
    OpenBabel::OBPlugin::ListAsVector("formats", nullptr, result);
    return std::move(result);
}

std::shared_ptr<Mol3D> MolUtilOpenBabelImpl::getCoord3D(const std::shared_ptr<JMol> &_mol) {
    auto obMol = convertJMolToOBMol(_mol);
    if (!runOBForceField(obMol, "uff")) {
        return nullptr;
    }
    auto mol3d = std::make_shared<Mol3D>(_mol);
    // TODO: 切换坐标
    return mol3d;
}

std::string MolUtilOpenBabelImpl::getFormat(const std::shared_ptr<JMol> &_mol, const std::string &_format) {
    auto obMol = convertJMolToOBMol(_mol);
    OpenBabel::OBConversion conv;
    auto formatOut = conv.FindFormat(_format);
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        std::cerr << "MolUtilOpenBabelImpl::getFormat: cannot set format!" << std::endl;
        exit(-1);
    }
    return conv.WriteString(&obMol);
}

std::shared_ptr<Mol3D> MolUtilOpenBabelImpl::fromFormat(const std::string &_content, const std::string &_format) {
    OpenBabel::OBConversion conv;
    auto formatIn = conv.FindFormat(_format);
    if (!formatIn || !conv.SetInFormat(formatIn)) {
        std::cerr << "MolUtilOpenBabelImpl::fromFormat: cannot set format!" << std::endl;
        exit(-1);
    }
    OpenBabel::OBMol obMol;
    std::stringstream ssm(_content);
    if (!conv.Read(&obMol, &ssm)) {
        std::cerr << "MolUtilOpenBabelImpl::fromFormat: cannot read:"
                  << _content << " as " << _format << std::endl;
        exit(-1);
    }
    auto mol3d = std::make_shared<Mol3D>(convertOBMolToJMol(obMol));
    // TODO: 如果文件包含坐标信息，填入mol3d
    return mol3d;
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getLSSR(const std::shared_ptr<JMol> &_mol) {
    // TODO: 实现之
    return std::vector<std::vector<size_t>>();
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getSSSR(const std::shared_ptr<JMol> &_mol) {
    // TODO: 实现之
    return std::vector<std::vector<size_t>>();
}
