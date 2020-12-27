#include "mol3d.hpp"
#include "qt_util.hpp"
#include "std_util.hpp"
#include "bond_type.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"

std::unordered_map<size_t, cv::Point3f> Mol3D::calcCoord3D(bool _updateAtomPosMap2D) {
    // TODO: 要求更新3D坐标的时候必须加氢
    if (_addHs) {
        if (atomValenceMap.empty()) {
            updateAtomValenceMap();
        }
        auto add_all_hydrogen = [&](const size_t &_aid) {
            addHs(_aid);
        };
        safeTraverseAtoms(add_all_hydrogen);
    }
    std::unordered_map<size_t, cv::Point3f> depAtomPosMap;
#ifdef USE_OPENBABEL
    OpenBabel::OBMol obMol;
    obMol.BeginModify();
    for(auto[id,atom]:atomsMap){
        auto obAtom = obMol.NewAtom(id);
        int atomicNumber = atom->getAtomicNumber();
        // bug occurs if add H here. add F instead。 REM to fix it later.
        if (atomicNumber == 1)atomicNumber = 9;
        obAtom->SetAtomicNum(atomicNumber);
    }
    for(auto[id,bond]:bondsMap){
        auto obBond = obMol.NewBond();
        obBond->SetBondOrder(std::lround(bond->asValence().floatValue()));
        obBond->SetBegin(obMol.GetAtomById(bond->getAtomFrom()));
        obBond->SetEnd(obMol.GetAtomById(bond->getAtomTo()));
    }
    obMol.EndModify();
    OpenBabel::OBBuilder builder;
    builder.Build(obMol);
    obMol.AddHydrogens(false, true);
    OpenBabel::OBForceField *pFF = OpenBabel::OBForceField::FindForceField("MMFF94s");
    if (!pFF) {
        std::cerr << "fail to find uff forcefield" << std::endl;
        exit(-1);
    }
    obMol.AddHydrogens(false, true);
    // this func return f
    pFF->Setup(obMol);
//    if (!pFF->Setup(obMol)) {
//        std::cerr << "fail to setup uff forcefield for mol" << std::endl;
//        exit(-1);
//    }
    pFF->SteepestDescent(250, 1.0e-4);
    pFF->WeightedRotorSearch(50, 25);
    pFF->SteepestDescent(500, 1.0e-6);
    pFF->UpdateCoordinates(obMol);
    FOR_ATOMS_OF_MOL(obAtom, obMol) {
        depAtomPosMap[obAtom->GetId()] = cv::Point3f(obAtom->x(), obAtom->y(), obAtom->z());
    }
#elif defined(USE_RDKIT)
    auto[rwMol, j2rAidMap] = convertJMolToRWMol(*this);
    std::unordered_map<unsigned int, size_t> r2jAidMap;
    for (auto&[jAid, rAid]:j2rAidMap) {
        r2jAidMap[rAid] = jAid;
    }
    j2rAidMap.clear();
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        rwMol->getAtomWithIdx(i)->calcImplicitValence(true);
        rwMol->getAtomWithIdx(i)->calcExplicitValence(true);
    }
//    RDKit::MolOps::findSSSR(*rwMol, nullptr);
    RDKit::MolOps::fastFindRings(*rwMol);
    int confId = RDKit::DGeomHelpers::EmbedMolecule(*rwMol, 1000, 1234);
//    auto[state, energy]=RDKit::UFF::UFFOptimizeMolecule(
//            *rwMol, 1000, 10.0, confId,true);
//    std::cout << "state=" << state << ",energy=" << energy << std::endl;
    RDKit::MMFF::MMFFOptimizeMolecule(*rwMol, 10000, "MMFF94", 10.0, confId);
    auto conf = rwMol->getConformer(confId);
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        auto pos = conf.getAtomPos(i);
        depAtomPosMap[r2jAidMap[i]] = cv::Point3f(pos.x, pos.y, pos.z);
    }
#endif //! USE_OPENBABEL
    return std::move(depAtomPosMap);
}
