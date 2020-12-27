#include "mol2d.hpp"
#include "opencv_util.hpp"

#include <coordgen/sketcherMinimizer.h>

Mol2D::Mol2D(std::shared_ptr<JMol> _mol) {

}

std::unordered_map<size_t, cv::Point2f> JMol::get2DCoordinates() const {
    std::unordered_map<size_t, cv::Point2f> depAtomPosMap;
    // 踩坑系列：sketcherMinimizer 析构的时候会 delete 所有关联的指针
    // 这里 sketcherMinimizerMolecule 一定得 new 一个，用智能指针或值就会报 double free
    // 我吃了一个苹果，苹果没了，我也没了
    sketcherMinimizer minimizer;
    auto sMol = new sketcherMinimizerMolecule();
    std::unordered_map<size_t, sketcherMinimizerAtom *> j2cAtomMap;
    for (auto&[id, atom]:atomsMap) {
        auto a = sMol->addNewAtom();
        a->setAtomicNumber(atom->getAtomicNumber());
        j2cAtomMap[id] = a;
    }
    std::vector<sketcherMinimizerBond *> cBondVec;
    for (auto&[id, bond]:bondsMap) {
        auto b = sMol->addNewBond(j2cAtomMap[bond->getAtomFrom()],
                                  j2cAtomMap[bond->getAtomTo()]);
        b->setBondOrder(std::lround(bond->asValence().floatValue()));
        cBondVec.push_back(b);
    }
    minimizer.initialize(sMol);
    minimizer.runGenerateCoordinates();
    for (auto&[aid, cAtom]:j2cAtomMap) {
        auto pos = cAtom->getCoordinates();
        depAtomPosMap[aid] = cv::Point2f(pos.x(), pos.y());
    }

    // FIXME: 想一下怎么设计函数的 const
    float &_fontSize = const_cast<float &>(fontSize);
    _fontSize = 0;
    for (auto&[_, bond]:bondsMap) {
        _fontSize += getDistance2D(depAtomPosMap[bond->getAtomFrom()],
                                   depAtomPosMap[bond->getAtomTo()]);
    }
    if (bondsMap.empty()) {
        _fontSize = 30;
    } else {
        _fontSize /= bondsMap.size();
    }
    return std::move(depAtomPosMap);
}