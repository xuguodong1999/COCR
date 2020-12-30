#include "openbabel_util.hpp"

#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/plugin.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

static std::unordered_map<size_t, OpenBabel::OBAtom *> j2oAtomMap;
static std::unordered_map<unsigned long, size_t> o2jAtomMap;

OpenBabel::OBMol convertJMolToOBMol(const std::shared_ptr<JMol> &_jMol) {
    OpenBabel::OBMol obMol;
    // FIXME: 这里的转换丢失了构型信息、没有包含离域键
    j2oAtomMap.clear();
    auto convert_atom = [&](const size_t &_aid) {
        auto obAtom = obMol.NewAtom();
        obAtom->SetAtomicNum(_jMol->getAtomById(_aid)->getAtomicNumber());
        j2oAtomMap[_aid] = obAtom;
    };
    _jMol->safeTraverseAtoms(convert_atom);
    auto convert_bond = [&](const size_t &_bid) {
        auto obBond = obMol.NewBond();
        auto bond = _jMol->getBondById(_bid);
        obBond->SetBondOrder(std::lround(bond->asValence().floatValue()));
        obBond->SetBegin(j2oAtomMap[bond->getAtomFrom()]);
        obBond->SetEnd(j2oAtomMap[bond->getAtomTo()]);
    };
    _jMol->safeTraverseBonds(convert_bond);
    return std::move(obMol);
}

std::shared_ptr<JMol> convertOBMolToJMol(const OpenBabel::OBMol &_obMol) {
    auto jMol = std::make_shared<JMol>();
    // TODO: 转换
    o2jAtomMap.clear();
    FOR_ATOMS_OF_MOL(obAtom, const_cast<OpenBabel::OBMol &>(_obMol)) {
        auto atom = jMol->addAtom(obAtom->GetAtomicNum());
        o2jAtomMap[obAtom->GetId()] = atom->getId();
    }
    FOR_BONDS_OF_MOL(obBond, const_cast<OpenBabel::OBMol &>(_obMol)) {
        auto aid1 = o2jAtomMap[obBond->GetBeginAtom()->GetId()];
        auto aid2 = o2jAtomMap[obBond->GetEndAtom()->GetId()];
        jMol->addBond(aid1, aid2, static_cast<JBondType>(obBond->GetBondOrder()));
    }
    return jMol;
}

bool runOBForceField(OpenBabel::OBMol &_obMol, const std::string &_forcefield = "uff") {
    using namespace OpenBabel;
    if (_obMol.Empty()) {
        std::cerr << "mol.Empty()" << std::endl;
        return false;
    }
    try {
        OBBuilder builder;
        builder.Build(_obMol);
        // FIXME: failed for C1CCCCC1
        _obMol.AddHydrogens(false, true);
    } catch (std::exception &e) {
        std::cerr << "runOBForceField build & add H step:" << e.what() << std::endl;
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
    pFF->SetLogLevel(OBFF_LOGLVL_NONE);
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

std::string MolUtilOpenBabelImpl::getFormat(
        const std::shared_ptr<JMol> &_mol, const std::string &_format) {
    OpenBabel::OBConversion conv;
    auto formatOut = conv.FindFormat(_format);
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        std::cerr << "MolUtilOpenBabelImpl::getFormat: cannot set format!" << std::endl;
        exit(-1);
    }
    std::cout<<"_mol->atomsNum()"<<_mol->atomsNum()<<std::endl;
    auto obMol = convertJMolToOBMol(_mol);
    if ("pdb" == _format) {
        std::cout<<"runOBForceField(obMol)="<<runOBForceField(obMol)<<std::endl;
    }
    std::cout<<"obMol.NumAtoms()"<<obMol.NumAtoms()<<std::endl;
    return conv.WriteString(&obMol);
}

std::shared_ptr<JMol> MolUtilOpenBabelImpl::fromFormat(const std::string &_content, const std::string &_format) {
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
        return nullptr;
    }
    // TODO: 如果文件包含坐标信息，填入mol3d
    return convertOBMolToJMol(obMol);
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getLSSR(const std::shared_ptr<JMol> &_mol) {
    // TODO: 实现之
    return std::vector<std::vector<size_t>>();
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getSSSR(const std::shared_ptr<JMol> &_mol) {
    // TODO: 实现之
    return std::vector<std::vector<size_t>>();
}
