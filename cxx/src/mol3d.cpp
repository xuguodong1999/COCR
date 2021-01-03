#include "mol3d.hpp"
#include "qt_util.hpp"
#include "std_util.hpp"
#include "bond_type.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"


std::unordered_map<size_t, cv::Point3f> Mol3D::calcCoord3D(bool _updateAtomPosMap2D) {
//    // 要求更新3D坐标的时候必须加氢
//
//        if (atomValenceMap.empty()) {
//            updateAtomValenceMap();
//        }
//        auto add_all_hydrogen = [&](const size_t &_aid) {
//            addHs(_aid);
//        };
//        safeTraverseAtoms(add_all_hydrogen);
//
    std::unordered_map<size_t, cv::Point3f> depAtomPosMap;
//
//
//    OpenBabel::OBMol obMol;
//    obMol.BeginModify();
//    for(auto[id,atom]:atomsMap){
//        auto obAtom = obMol.NewAtom(id);
//        int atomicNumber = atom->getAtomicNumber();
//        // bug occurs if add H here. add F instead。 REM to fix it later.
//        if (atomicNumber == 1)atomicNumber = 9;
//        obAtom->SetAtomicNum(atomicNumber);
//    }
//    for(auto[id,bond]:bondsMap){
//        auto obBond = obMol.NewBond();
//        obBond->SetBondOrder(std::lround(bond->asValence().floatValue()));
//        obBond->SetBegin(obMol.GetAtomById(bond->getAtomFrom()));
//        obBond->SetEnd(obMol.GetAtomById(bond->getAtomTo()));
//    }
//    obMol.EndModify();
//    OpenBabel::OBBuilder builder;
//    builder.Build(obMol);
//    obMol.AddHydrogens(false, true);
//    OpenBabel::OBForceField *pFF = OpenBabel::OBForceField::FindForceField("MMFF94s");
//    if (!pFF) {
//        std::cerr << "fail to find uff forcefield" << std::endl;
//        exit(-1);
//    }
//    obMol.AddHydrogens(false, true);
//    // this func return f
//    pFF->Setup(obMol);
////    if (!pFF->Setup(obMol)) {
////        std::cerr << "fail to setup uff forcefield for mol" << std::endl;
////        exit(-1);
////    }
//    pFF->SteepestDescent(250, 1.0e-4);
//    pFF->WeightedRotorSearch(50, 25);
//    pFF->SteepestDescent(500, 1.0e-6);
//    pFF->UpdateCoordinates(obMol);
//    FOR_ATOMS_OF_MOL(obAtom, obMol) {
//        depAtomPosMap[obAtom->GetId()] = cv::Point3f(obAtom->x(), obAtom->y(), obAtom->z());
//    }
//
    return std::move(depAtomPosMap);
}

Mol3D::Mol3D(std::shared_ptr<JMol> _originMol, std::shared_ptr<JMol> _molWithHydrogens)
:MolBase(_originMol,_molWithHydrogens){

}

Mol3D::Mol3D(std::shared_ptr<JMol> _mol) : MolBase(std::move(_mol)) {
}
