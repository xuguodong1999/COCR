#include "ckit/mol.h"
#include "deprecated/jmol_adapter.h"
#include <memory>

GuiMol::GuiMol() : m(std::make_shared<ckit_deprecated::JMolAdapter>()) {
}

GuiMol::GuiMol(GuiMol &&mol) noexcept: m(std::move(mol.m)) {
}

GuiMol::GuiMol(const GuiMol &mol) : m(mol.m->deepClone()) {
}


gui_mol GuiMol::deepClone() const {
    auto m2 = std::make_shared<GuiMol>();
    m2->m = this->m->deepClone();
    return m2;
}

void GuiMol::addAllHydrogens() {
    m->addAllHydrogens();
}

bool GuiMol::tryExpand() {
    return m->tryExpand();
}

void GuiMol::set2DInfoLatest(bool is2DInfoLatest) {
    m->set2DInfoLatest(is2DInfoLatest);
}

void GuiMol::norm2D(
        const float &w, const float &h,
        const float &x, const float &y, bool keepRatio) {
    m->norm2D(w, h, x, y, keepRatio);
}

void GuiMol::norm3D(
        const float &xx, const float &yy, const float &zz,
        const float &x, const float &y, const float &z,
        bool keepRatio) {
    m->norm3D(xx, yy, zz, x, y, z, keepRatio);
}

float GuiMol::getAvgBondLength() {
    return m->getAvgBondLength();
}

void GuiMol::loopAtomVec(std::function<void(Atom &)> func) {
    m->loopAtomVec(std::move(func));
}

void GuiMol::loopBondVec(std::function<void(Bond &)> func) {
    m->loopBondVec(std::move(func));
}

std::shared_ptr<Atom> GuiMol::getAtom(const id_type &aid) {
    return m->getAtom(aid);
}

std::shared_ptr<Bond> GuiMol::getBond(const id_type &bid) {
    return m->getBond(bid);
}

std::shared_ptr<Atom> GuiMol::addAtom(
        const ElementType &element, const float &x, const float &y, const float &z) {
    return m->addAtom(element, x, y, z);
}

std::shared_ptr<Bond> GuiMol::addBond(
        std::shared_ptr<Atom> a1, std::shared_ptr<Atom> a2,
        const BondType &type,
        const float &offset1, const float &offset2) {
    return m->addBond(a1, a2, type, offset1, offset2);
}

std::string GuiMol::writeAs(const std::string &suffix) {
    return m->writeAs(suffix);
}

std::shared_ptr<Atom> GuiMol::addSuperAtom(
        const std::string &name,
        const float &x0, const float &y0,
        const float &x1, const float &y1) {
    return m->addSuperAtom(name, x0, y0, x1, y1);
}

std::vector<std::vector<id_type>> GuiMol::getSSSR() {
    return m->getSSSR();
}

void GuiMol::tryMarkDoubleBond(const id_type &bid) {
    m->tryMarkDoubleBond(bid);
}



