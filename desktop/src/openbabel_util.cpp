#include "openbabel_util.hpp"

#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/ring.h>
#include <openbabel/obiter.h>
#include <openbabel/plugin.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

#include <QDebug>

static std::unordered_map<size_t, OpenBabel::OBAtom *> j2oAtomMap;
static std::unordered_map<unsigned long, size_t> o2jAtomMap;

OpenBabel::OBMol convertJMolToOBMol(const JMol &_jMol) {
    OpenBabel::OBMol obMol;
    // FIXME: 这里的转换丢失了构型信息、没有包含离域键
    j2oAtomMap.clear();
    o2jAtomMap.clear();
    auto convert_atom = [&](const size_t &_aid) {
        auto obAtom = obMol.NewAtom();
        obAtom->SetAtomicNum(_jMol.getAtomById(_aid)->getAtomicNumber());
        j2oAtomMap[_aid] = obAtom;
        o2jAtomMap[obAtom->GetId()] = _aid;
    };
    _jMol.safeTraverseAtoms(convert_atom);
    auto convert_bond = [&](const size_t &_bid) {
        auto obBond = obMol.NewBond();
        auto bond = _jMol.getBondById(_bid);
        obBond->SetBegin(j2oAtomMap[bond->getAtomFrom()]);
        obBond->SetEnd(j2oAtomMap[bond->getAtomTo()]);
        if (JBondType::DelocalizedBond == bond->getBondType()) {
            obBond->SetBondOrder(5);
        } else {
            obBond->SetBondOrder(std::lround(bond->asValence().floatValue()));
        }
    };
    _jMol.safeTraverseBonds(convert_bond);
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
        JBondType bondType;
        if (5 == obBond->GetBondOrder()) {
            bondType = JBondType::DelocalizedBond;
        } else {
            bondType = static_cast<JBondType>(obBond->GetBondOrder());
        }
        jMol->addBond(aid1, aid2, bondType);
    }
    return jMol;
}

bool runOBForceField(OpenBabel::OBMol &_obMol, const std::string &_forcefield = "uff") {
    using namespace OpenBabel;
    if (_obMol.Empty()) {
        qDebug() << "mol.Empty()";
        return false;
    }
    try {
        OBBuilder builder;
        builder.Build(_obMol);
        // FIXME: add or delete hydrogens dont wrok
        _obMol.AddHydrogens(false, true);
    } catch (std::exception &e) {
        qDebug() << "runOBForceField build & add H step:" << e.what();
        return false;
    }
    auto pFF = static_cast<OBForceField *>(OBPlugin::GetPlugin(
            "forcefields", _forcefield.c_str()));
    if (!pFF) {
        qDebug() << "runOBForceField: could not find forcefield '"
                 << _forcefield.c_str() << "'.";
        return false;
    }
    pFF->SetLogLevel(OBFF_LOGLVL_NONE);
    if (!pFF->Setup(_obMol)) {
        qDebug() << "runOBForceField: setup force field ret false";
//        return false;
    }
    try {
        pFF->SteepestDescent(100, 1.0e-4);
        pFF->WeightedRotorSearch(10, 200);
        pFF->SteepestDescent(100, 1.0e-6);
        pFF->UpdateCoordinates(_obMol);
    } catch (std::exception &e) {
        qDebug() << "runOBForceField MD step:" << e.what();
        return false;
    }
    return true;
}


std::string MolUtilOpenBabelImpl::getSMILES(const JMol &_mol) {
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

extern std::string pdb_str, sdf_str, xyz_str, smi_str;

bool MolUtilOpenBabelImpl::getCoord3D(Mol3D &_mol3d) {
    auto &jMol = *_mol3d.getMol();
    auto obMol = convertJMolToOBMol(jMol);
    if (!runOBForceField(obMol, "uff")) {
        return false;
    }
    _mol3d.reset(convertOBMolToJMol(obMol));
    FOR_ATOMS_OF_MOL(obAtom, const_cast<OpenBabel::OBMol &>(obMol)) {
        _mol3d.setAtomPos3DById(o2jAtomMap[obAtom->GetId()],
                                obAtom->x(), obAtom->y(), obAtom->z());
    }
    OpenBabel::OBConversion conv;
    auto formatOut = conv.FindFormat("pdb");
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        qDebug() << "MolUtilOpenBabelImpl::getFormat: cannot set format!";
    }
    pdb_str = conv.WriteString(&obMol);
    formatOut = conv.FindFormat("xyz");
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        qDebug() << "MolUtilOpenBabelImpl::getFormat: cannot set format!";
    }
    xyz_str = conv.WriteString(&obMol);
    formatOut = conv.FindFormat("sdf");
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        qDebug() << "MolUtilOpenBabelImpl::getFormat: cannot set format!";
    }
    sdf_str = conv.WriteString(&obMol);
    formatOut = conv.FindFormat("smi");
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        qDebug() << "MolUtilOpenBabelImpl::getFormat: cannot set format!";
    }
    smi_str = conv.WriteString(&obMol);
    return true;
}

std::string MolUtilOpenBabelImpl::getFormat(
        const JMol &_mol, const std::string &_format) {
    OpenBabel::OBConversion conv;
    auto formatOut = conv.FindFormat(_format);
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        qDebug() << "MolUtilOpenBabelImpl::getFormat: cannot set format!";
        exit(-1);
    }
    std::cout << "_mol->atomsNum()" << _mol.atomsNum();
    auto obMol = convertJMolToOBMol(_mol);
    if ("pdb" == _format) {
        std::cout << "runOBForceField(obMol)=" << runOBForceField(obMol);
    }
    return conv.WriteString(&obMol);
}

std::shared_ptr<JMol> MolUtilOpenBabelImpl::fromFormat(const std::string &_content, const std::string &_format) {
    OpenBabel::OBConversion conv;
    auto formatIn = conv.FindFormat(_format);
    if (!formatIn || !conv.SetInFormat(formatIn)) {
        qDebug() << "MolUtilOpenBabelImpl::fromFormat: cannot set format!";
        exit(-1);
    }
    OpenBabel::OBMol obMol;
    std::stringstream ssm(_content);
    if (!conv.Read(&obMol, &ssm)) {
        qDebug() << "MolUtilOpenBabelImpl::fromFormat: cannot read:"
                 << _content.c_str() << " as " << _format.c_str();
        return nullptr;
    }
    // TODO: 如果文件包含坐标信息，填入mol3d
    return convertOBMolToJMol(obMol);
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getLSSR(const JMol &_mol) {
    auto obMol = convertJMolToOBMol(_mol);
    auto &rings = obMol.GetLSSR();
    std::vector<std::vector<size_t>> retRings(rings.size());
    for (size_t i = 0; i < rings.size(); i++) {
        auto &ring = rings[i];
        for (auto &idx:ring->_path) {
            retRings[i].push_back(o2jAtomMap[obMol.GetAtom(idx)->GetId()]);
        }
    }
    return std::move(retRings);
}

std::vector<std::vector<size_t>> MolUtilOpenBabelImpl::getSSSR(const JMol &_mol) {
    auto obMol = convertJMolToOBMol(_mol);
    auto &rings = obMol.GetSSSR();
    std::vector<std::vector<size_t>> retRings(rings.size());
    for (size_t i = 0; i < rings.size(); i++) {
        auto &ring = rings[i];
        for (auto &idx:ring->_path) {
            retRings[i].push_back(o2jAtomMap[obMol.GetAtom(idx)->GetId()]);
        }
    }
    return std::move(retRings);
}
