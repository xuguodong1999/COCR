#include "mol2d.hpp"
#include "opencv_util.hpp"

#include <coordgen/sketcherMinimizer.h>

Mol2D::Mol2D(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {

}

void Mol2D::calcCoord2D() {
    // 踩坑系列：sketcherMinimizer 析构的时候会 delete 所有关联的指针
    // 这里 sketcherMinimizerMolecule 一定得 new 一个，用智能指针或值就会报 double free
    sketcherMinimizer minimizer;
    auto cMol = new sketcherMinimizerMolecule();
    std::unordered_map<size_t, sketcherMinimizerAtom *> j2cAtomMap;
    mol->safeTraverseAtoms([&](const size_t &aid) {
        auto cAtom = cMol->addNewAtom();
        cAtom->setAtomicNumber(mol->getAtomById(aid)->getAtomicNumber());
        j2cAtomMap[aid] = cAtom;
    });
    std::vector<sketcherMinimizerBond *> cBondVec;
    mol->safeTraverseBonds([&](const size_t &bid) {
        auto jBond = mol->getBondById(bid);
        auto cBond = cMol->addNewBond(j2cAtomMap[jBond->getAtomFrom()],
                                      j2cAtomMap[jBond->getAtomTo()]);
        cBond->setBondOrder(std::lround(jBond->asValence().floatValue()));
        cBondVec.push_back(cBond);
    });
    minimizer.initialize(cMol);
    minimizer.runGenerateCoordinates();
    atomPosMap2D.clear();
    for (auto&[aid, cAtom]:j2cAtomMap) {
        auto pos = cAtom->getCoordinates();
        atomPosMap2D[aid] = {true, cv::Point2f(pos.x(), pos.y())};
    }
}

float Mol2D::getAvgBondLength() const {
    if (mol->IsBondsEmpty()) {
        return 100;
    }
    float avgBondLength = 0;
    mol->safeTraverseBonds([&](const size_t &bid) {
        auto bond = mol->getBondById(bid);
        avgBondLength += getDistance2D(getPos2D(bond->getAtomFrom()), getPos2D(bond->getAtomTo()));
    });
    return avgBondLength / mol->bondsNum();
}

void Mol2D::setPos2D(const size_t &_aid, bool _isExplicit, const cv::Point2f &_pos) {
    atomPosMap2D[_aid] = {_isExplicit, _pos};
}

const cv::Point2f &Mol2D::getPos2D(const size_t &_aid) const {
    return atomPosMap2D.find(_aid)->second.second;
}

bool Mol2D::isExplicit2D(const size_t &_aid) const {
    return atomPosMap2D.find(_aid)->second.first;
}
