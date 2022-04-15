#include "jmol.h"
#include "jmol_p.h"
#include <cmath>
#include <iostream>
#include <exception>

using namespace v1_1;
// 禁用了一些显示不了 c1ccccc1 的格式
std::unordered_set<std::string>v1_1::JMol::FORMAT_WRITE_WHITE_LIST = {
        "acesin", "adf", "alc", "ascii", "bgf", "box", "bs", "c3d1", "c3d2", "cac", "caccrt",
        "cache", "cacint", "can", "cdjson", "cht", "cif", "ck", "cof", "com",
//        "confabreport",
        "CONFIG", "CONTCAR", "CONTFF",
//        "copy",
        "crk2d", "crk3d",
//        "csr",
        "cssr", "ct",
//        "cub", "cube",
        "dalmol", "dmol",
//        "dx",
        "ent", "exyz",
//        "fa", "fasta",
        "feat", "fh", "fhiaims", "fix", "fps", "fpt", "fract",
//        "fs", "fsa",
        "gamin", "gau", "gjc", "gjf", "gpr", "gr96", "gro", "gukin",
//        "gukout",
        "gzmat", "hin", "inchi", "inchikey", "inp", "jin", "k", "lmpdat",
//        "lpmd",
        "mae", "mcdl", "mcif", "MDFF", "mdl", "ml2", "mmcif", "mmd", "mmod", "mna", "mol", "mol2", "mold", "molden",
        "molf", "molreport", "mop", "mopcrt", "mopin", "mp", "mpc", "mpd", "mpqcin", "msms",
//        "nul",
        "nw", "orcainp", "outmol", "paint", "pcjson", "pcm", "pdb", "pdbqt", "pointcloud", "POSCAR", "POSFF",
//        "pov",
        "pqr", "pqs", "qcin", "report",
//        "rinchi", "rsmi", "rxn",
        "sd", "sdf", "smi", "smiles",
//        "stl",
        "svg", "sy2",
//        "tdd", "text", "therm",
        "tmol",
//        "txt",
        "txyz", "unixyz",
        "VASP", "vmol", "xed", "xyz",
//        "yob",
        "zin"
};

std::shared_ptr<Atom> v1_1::JMol::getAtom(const id_type &_aid) {
    auto it = atomMap.find(_aid);
    if (atomMap.end() == it) { return nullptr; }
    return it->second;
}

std::shared_ptr<Bond> v1_1::JMol::getBond(const id_type &_bid) {
    auto it = bondMap.find(_bid);
    if (bondMap.end() == it) { return nullptr; }
    return it->second;
}


std::shared_ptr<Atom> JMol::addAtom(const ElementType &_element, const float &_x, const float &_y) {
    _p->exceedValence();
    auto atom = std::make_shared<Atom>(idBase++, _element, _x, _y);
    atomMap[atom->getId()] = atom;
    return atom;
}

std::shared_ptr<Atom> JMol::addAtom(
        const ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}


std::shared_ptr<Atom> JMol::removeAtom(const id_type &_aid) {
    _p->exceedValence();
    auto atom = getAtom(_aid);
    if (atom) { atomMap.erase(_aid); }
    return atom;
}

std::shared_ptr<Bond> JMol::removeBond(const id_type &_bid) {
    _p->exceedValence();
    auto bond = getBond(_bid);
    if (bond) { bondMap.erase(_bid); }
    return bond;
}

void JMol::setId(const id_type &_id) {
    id = _id;
}

id_type JMol::getId() {
    return id;
}

JMol::JMol() : id(0), is3DInfoLatest(false), is2DInfoLatest(false), idBase(0),
               _p(std::make_shared<JMol_p>(*this)) {

}

void JMol::loopAtomVec(std::function<void(Atom &)> _func) {
    for (auto &[aid, atom]: atomMap) {
        if (atom) { _func(*atom); }
    }
}

void JMol::loopBondVec(std::function<void(Bond &)> _func) {
    for (auto &[bid, bond]: bondMap) {
        if (bond) { _func(*bond); }
    }
}

std::shared_ptr<Atom> JMol::addAtom(const int &_atomicNumber) {
    _p->exceedValence();
    auto atom = std::make_shared<Atom>(idBase++, static_cast<ElementType>(_atomicNumber));
    atomMap[atom->getId()] = atom;
    return atom;
}

void JMol::display() {
    onExtraDataNeeded();
    loopAtomVec([&](Atom &atom) {
        std::cerr << atom.getId() << ":" << atom.getName().c_str();
        if (is2DInfoLatest) {
            std::cerr << "2d(" << atom.x << "," << atom.y << ")";
        }
        if (is3DInfoLatest) {
            std::cerr << "3d(" << atom.xx << "," << atom.yy << atom.zz << ")";
        }
    });
    loopBondVec([&](Bond &bond) {
        std::cerr << bond.getId() << ":" << bond.getBondOrder() << ","
                  << bond.getFrom()->getId() << "-" << bond.getTo()->getId();
    });
}

void JMol::onExtraDataNeeded() {
    // default behavior: do nothing
}

void JMol::norm2D(const float &_w, const float &_h, const float &_x, const float &_y, bool keepRatio) {
    if (!is2DInfoLatest) {
        generate2D();
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    loopAtomVec([&](Atom &_atom) {
        minx = std::min(minx, _atom.x);
        miny = std::min(miny, _atom.y);
        maxx = std::max(maxx, _atom.x);
        maxy = std::max(maxy, _atom.y);
    });
    float kw = (_w - _x * 3) / (maxx - minx), kh = (_h - _y * 3) / (maxy - miny);
    if (keepRatio) {
        float k = std::min(kw, kh);
        loopAtomVec([&](Atom &_atom) {
            _atom.x = (_atom.x - minx) * k + _x;
            _atom.y = (_atom.y - miny) * k + _y;

            _atom.x0 = (_atom.x0 - minx) * k + _x;
            _atom.y0 = (_atom.y0 - miny) * k + _y;
            _atom.x1 = (_atom.x1 - minx) * k + _x;
            _atom.y1 = (_atom.y1 - miny) * k + _y;
        });
    } else {
        loopAtomVec([&](Atom &_atom) {
            _atom.x = (_atom.x - minx) * kw + _x;
            _atom.y = (_atom.y - miny) * kh + _y;

            _atom.x0 = (_atom.x0 - minx) * kw + _x;
            _atom.y0 = (_atom.y0 - miny) * kh + _y;
            _atom.x1 = (_atom.x1 - minx) * kw + _x;
            _atom.y1 = (_atom.y1 - miny) * kh + _y;
        });
    }
}

void JMol::norm3D(const float &_xx, const float &_yy, const float &_zz,
                  const float &_x, const float &_y, const float &_z, bool keepRatio) {
    std::cerr << __FUNCTION__;
    if (!is3DInfoLatest) {
        std::cerr << __FUNCTION__ << "generate3D=" << generate3D();
    }
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::max();
    maxx = maxy = maxz = std::numeric_limits<float>::lowest();
    loopAtomVec([&](Atom &_atom) {
        minx = std::min(minx, _atom.xx);
        miny = std::min(miny, _atom.yy);
        minz = std::min(minz, _atom.zz);
        maxx = std::max(maxx, _atom.xx);
        maxy = std::max(maxy, _atom.yy);
        maxz = std::max(maxz, _atom.zz);
    });
    float kx = (_xx - _x * 2) / (maxx - minx), ky = (_yy - _y * 2) / (maxy - miny), kz = (_zz - _z * 2) / (maxz - minz);
    float dx = (minx + maxx) / 2, dy = (miny + maxy) / 2, dz = (minz + maxz) / 2;
    if (keepRatio) {
        float k = (std::min)(kx, (std::min)(ky, kz));
        loopAtomVec([&](Atom &_atom) {
            _atom.xx = (_atom.xx - dx) * k + _x;
            _atom.yy = (_atom.yy - dy) * k + _y;
            _atom.zz = (_atom.zz - dz) * k + _z;
        });
    } else {
        loopAtomVec([&](Atom &_atom) {
            _atom.xx = (_atom.xx - dx) * kx + _x;
            _atom.yy = (_atom.yy - dy) * ky + _y;
            _atom.zz = (_atom.zz - dz) * kz + _y;
        });
    }
}

float JMol::getAvgBondLength() {
    std::cerr << __FUNCTION__;
    if (bondMap.empty()) { return 0; }
    if (!is3DInfoLatest) {
        std::cerr << __FUNCTION__ << "generate3D=" << generate3D();
    }
    float avgBondLength = 0;
    loopBondVec([&](Bond &bond) {
        auto from = bond.getFrom(), to = bond.getTo();
        avgBondLength += std::sqrt(std::pow(from->xx - to->xx, 2) +
                                   std::pow(from->yy - to->yy, 2) +
                                   std::pow(from->zz - to->zz, 2));
    });
    return avgBondLength / bondMap.size();
}

float JMol::getAvgBondLength2D() {
    float avgBondLength = 0;
    loopBondVec([&](Bond &bond) {
        auto from = bond.getFrom(), to = bond.getTo();
        avgBondLength += std::sqrt(std::pow(from->x - to->x, 2) +
                                   std::pow(from->y - to->y, 2));
    });
    return avgBondLength / bondMap.size();
}

size_t JMol::getBondNum() const {
    return bondMap.size();
}

size_t JMol::getAtomNum() const {
    return atomMap.size();
}

void JMol::set2DInfoLatest(bool _is2DInfoLatest) {
    is2DInfoLatest = _is2DInfoLatest;
}


std::shared_ptr<Bond> JMol::addBond(
        std::shared_ptr<Atom> _a1, std::shared_ptr<Atom> _a2, const BondType &_type,
        const float &_offset1, const float &_offset2) {
    _p->exceedValence();
    auto bond = std::make_shared<Bond>(idBase++, _a1, _a2, _type, _offset1, _offset2);
    bondMap[bond->getId()] = bond;
    return bond;
}

std::shared_ptr<Atom> JMol::addSuperAtom(
        const std::string &_name, const float &_x0, const float &_y0,
        const float &_x1, const float &_y1) {
    _p->exceedValence();
    auto atom = std::make_shared<Atom>(idBase++, _name, _x0, _y0, _x1, _y1);
    atomMap[atom->getId()] = atom;
    return atom;
}


void JMol::addAllHydrogens() {
    // 根据原子的键级累加表加氢
    // 情况处理：
    // 1、ELEMENT_COMMON_NEB_NUM_MAP 里没有记录的，一律不加氢
    // 2、硬编码特别处理常用原子带电荷的情况，如铵正离子、碳正离子、碳负离子、氧负离子
    // 3、其它情况按照 ELEMENT_COMMON_NEB_NUM_MAP 的记录默认处理
    loopCurrentAtomPtrVec([&](std::shared_ptr<Atom> _atom) -> void {
        int numHs = getNumHydrogen(_atom->getId());
        if (numHs < 1) { return; }
        while (numHs--) {
            auto h = addAtom(ElementType::H);
            auto bond = addBond(_atom, h);
            _p->addBondOrder4Atom(h->getId(), 1);
            _p->addBondOrder4Atom(_atom->getId(), 1);
        }
    });
    // 这里我之前有维护好加氢数据
    _p->confirmValence();
}

void JMol::loopCurrentAtomPtrVec(std::function<void(std::shared_ptr<Atom>)> _func) {
    std::vector<std::shared_ptr<Atom>> currentAtomPtr;
    for (auto &[aid, atom]: atomMap) {
        currentAtomPtr.push_back(atom);
    }
    for (auto &atom: currentAtomPtr) {
        if (atom) { _func(atom); }
    }
}

bool JMol::tryExpand() {
    bool ok = true;
    auto expand = [&](std::shared_ptr<Atom> atom) {
        if (!atom) { return; }
        if (ElementType::SA != atom->getType()) { return; }
        if (!_p->tryExpand(atom->getId())) { ok = false; }
    };
    loopCurrentAtomPtrVec(expand);
    return ok;
}

bool JMol::IsValidWritableFormat(const std::string &_suffix) {
    return FORMAT_WRITE_WHITE_LIST.end() != FORMAT_WRITE_WHITE_LIST.find(_suffix);
}


void JMol::tryMarkDoubleBond(const id_type &_bid) {
    auto bond = getBond(_bid);
    if (!bond) { return; }
    auto from = bond->getFrom(), to = bond->getTo();
    if (getNumHydrogen(from->getId()) >= 1 && getNumHydrogen(to->getId()) >= 1 &&
        !_p->getDoubleBondNum(from->getId()) && !_p->getDoubleBondNum(to->getId())) {
        bond->setType(BondType::DoubleBond);
        _p->addBondOrder4Atom(from->getId(), 1);
        _p->addBondOrder4Atom(to->getId(), 1);
        _p->addDoubleBondNum4Atom(from->getId(), 1);
        _p->addDoubleBondNum4Atom(to->getId(), 1);
    }
}

int JMol::getNumHydrogen(const id_type &_aid) {
    if (!_p->IsValenceDataLatest()) {
        _p->updateValenceMap();
    }
    auto atom = getAtom(_aid);
    if (!atom) { return 0; }
    auto result = ElementUtil::GetElementNebNum(atom->getType());
    // 常用原子邻接键级表中没有的原子不加氢
    if (!result) { return 0; }
    int order = _p->getAtomOrder(atom->getId());
    int numHs = result.value() - order;
    // 对电荷的特别处理
    int charge = atom->getCharge();
    if (0 != charge) {
        switch (atom->getType()) {
            case ElementType::C: {
                if (charge == 1) {// 碳正离子
                    numHs -= 1;
                } else if (charge == -1) {// 碳负离子
                    numHs -= 1;
                }
                break;
            }
            case ElementType::N: {
                if (charge == 1) {// 铵正离子
                    numHs += 1;
                } else {
                    numHs -= std::floor(charge / 2.0);
                }
                break;
            }
            case ElementType::O: {
                if (charge == -1) {
                    numHs -= 1;
                }
                break;
            }
            default: {
                numHs -= std::floor(charge / 2.0);
                break;
            }
        }
    }
    std::cerr << atom->getName() << numHs;
    return numHs;
}

void JMol::exceedAllData() {
    is3DInfoLatest = is2DInfoLatest = false;
}



