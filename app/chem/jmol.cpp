#include "jmol.hpp"
#include <QDebug>
#include <cmath>
#include <iostream>
#include <exception>

using namespace xgd;
// 禁用了一些显示不了 c1ccccc1 的格式
std::unordered_set<std::string>xgd::JMol::FORMAT_WRITE_WHITE_LIST = {
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
        "mcdl", "mcif", "MDFF", "mdl", "ml2", "mmcif", "mmd", "mmod", "mna", "mol", "mol2", "mold", "molden",
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

std::shared_ptr<JAtom> xgd::JMol::getAtom(const id_type &_aid) {
    auto it = atomMap.find(_aid);
    if (atomMap.end() == it) { return nullptr; }
    return it->second;
}

std::shared_ptr<JBond> xgd::JMol::getBond(const id_type &_bid) {
    auto it = bondMap.find(_bid);
    if (bondMap.end() == it) { return nullptr; }
    return it->second;
}


std::shared_ptr<JAtom> JMol::addAtom(const ElementType &_element, const float &_x, const float &_y) {
    auto atom = std::make_shared<JAtom>(idBase++, _element, _x, _y);
    atomMap[atom->getId()] = atom;
    return atom;
}

std::shared_ptr<JAtom> JMol::addAtom(
        const ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}


std::shared_ptr<JAtom> JMol::removeAtom(const id_type &_aid) {
    auto atom = getAtom(_aid);
    if (atom) { atomMap.erase(_aid); }
    return atom;
}

std::shared_ptr<JBond> JMol::removeBond(const id_type &_bid) {
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

JMol::JMol() : id(0), is3DInfoLatest(false), is2DInfoLatest(false), startAddingHydrogens(false), idBase(0) {

}

void JMol::loopAtomVec(std::function<void(JAtom &)> _func) {
    for (auto &[aid, atom]:atomMap) {
        if (atom) { _func(*atom); }
    }
}

void JMol::loopBondVec(std::function<void(JBond &)> _func) {
    for (auto &[bid, bond]:bondMap) {
        if (bond) { _func(*bond); }
    }
}

std::shared_ptr<JAtom> JMol::addAtom(const int &_atomicNumber) {
    auto atom = std::make_shared<JAtom>(idBase++, static_cast<ElementType>(_atomicNumber));
    atomMap[atom->getId()] = atom;
    return atom;
}

void JMol::display() {
    onExtraDataNeeded();
    loopAtomVec([&](JAtom &atom) {
        qDebug() << atom.getId() << ":" << atom.getName().c_str();
        if (is2DInfoLatest) {
            qDebug() << "2d(" << atom.x << "," << atom.y << ")";
        }
        if (is3DInfoLatest) {
            qDebug() << "3d(" << atom.xx << "," << atom.yy << atom.zz << ")";
        }
    });
    loopBondVec([&](JBond &bond) {
        qDebug() << bond.getId() << ":" << bond.getBondOrder() << ","
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
    loopAtomVec([&](JAtom &_atom) {
        minx = std::min(minx, _atom.x);
        miny = std::min(miny, _atom.y);
        maxx = std::max(maxx, _atom.x);
        maxy = std::max(maxy, _atom.y);
    });
    float kw = (_w - _x * 3) / (maxx - minx), kh = (_h - _y * 3) / (maxy - miny);
    if (keepRatio) {
        float k = std::min(kw, kh);
        loopAtomVec([&](JAtom &_atom) {
            _atom.x = (_atom.x - minx) * k + _x;
            _atom.y = (_atom.y - miny) * k + _y;

            _atom.x0 = (_atom.x0 - minx) * k + _x;
            _atom.y0 = (_atom.y0 - miny) * k + _y;
            _atom.x1 = (_atom.x1 - minx) * k + _x;
            _atom.y1 = (_atom.y1 - miny) * k + _y;
        });
    } else {
        loopAtomVec([&](JAtom &_atom) {
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
    qDebug() << __FUNCTION__;
    if (!is3DInfoLatest) {
        qDebug() << __FUNCTION__ << "generate3D=" << generate3D();
    }
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::max();
    maxx = maxy = maxz = std::numeric_limits<float>::lowest();
    loopAtomVec([&](JAtom &_atom) {
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
        float k = std::min(kx, std::min(ky, kz));
        loopAtomVec([&](JAtom &_atom) {
            _atom.xx = (_atom.xx - dx) * k + _x;
            _atom.yy = (_atom.yy - dy) * k + _y;
            _atom.zz = (_atom.zz - dz) * k + _z;
        });
    } else {
        loopAtomVec([&](JAtom &_atom) {
            _atom.xx = (_atom.xx - dx) * kx + _x;
            _atom.yy = (_atom.yy - dy) * ky + _y;
            _atom.zz = (_atom.zz - dz) * kz + _y;
        });
    }
}

float JMol::getAvgBondLength() {
    qDebug() << __FUNCTION__;
    if (bondMap.empty()) { return 0; }
    if (!is3DInfoLatest) {
        qDebug() << __FUNCTION__ << "generate3D=" << generate3D();
    }
    float avgBondLength = 0;
    loopBondVec([&](JBond &bond) {
        auto from = bond.getFrom(), to = bond.getTo();
        avgBondLength += std::sqrt(std::pow(from->xx - to->xx, 2) +
                                   std::pow(from->yy - to->yy, 2) +
                                   std::pow(from->zz - to->zz, 2));
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


std::shared_ptr<JBond> JMol::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2, const BondType &_type,
        const float &_offset1, const float &_offset2) {
    auto bond = std::make_shared<JBond>(idBase++, _a1, _a2, _type, _offset1, _offset2);
    bondMap[bond->getId()] = bond;
    return bond;
}

std::shared_ptr<JAtom> JMol::addSuperAtom(
        const std::string &_name, const float &_x0, const float &_y0,
        const float &_x1, const float &_y1) {
    auto atom = std::make_shared<JAtom>(idBase++, _name, _x0, _y0, _x1, _y1);
    atomMap[atom->getId()] = atom;
    return atom;
}


void JMol::addAllHydrogens() {
    std::unordered_map<id_type, int> atomTotalBondOrderMap;
    auto add_bond_order_for_atom = [&](const id_type &_aid, const int &_order) -> void {
        auto it = atomTotalBondOrderMap.find(_aid);
        if (atomTotalBondOrderMap.end() == it) {
            atomTotalBondOrderMap[_aid] = _order;
        } else {
            it->second += _order;
        }
    };
    auto get_atom_order = [&](const id_type &_aid) -> int {
        auto it = atomTotalBondOrderMap.find(_aid);
        if (atomTotalBondOrderMap.end() == it) {
            atomTotalBondOrderMap[_aid] = 0;
            return 0;
        } else {
            return it->second;
        }
    };
    // 初始化原子邻接键的键级累加表
    loopBondVec([&](JBond &_bond) {
        int order = _bond.getBondOrder();
        auto from = _bond.getFrom();
        auto to = _bond.getTo();
        if (from && to) {
            add_bond_order_for_atom(from->getId(), order);
            add_bond_order_for_atom(to->getId(), order);
        }
    });
    // 根据原子的键级累加表加氢
    // 情况处理：
    // 1、ELEMENT_COMMON_NEB_NUM_MAP 里没有记录的，一律不加氢
    // 2、硬编码特别处理常用原子带电荷的情况，如铵正离子、碳正离子、碳负离子、氧负离子
    // 3、其它情况按照 ELEMENT_COMMON_NEB_NUM_MAP 的记录默认处理
    setStartAddingHydrogens(true);
    loopCurrentAtomPtrVec([&](std::shared_ptr<JAtom> _atom) -> void {
        // 氢原子不加氢
        if (ElementType::H == _atom->getType()) { return; }
        auto it = ELEMENT_COMMON_NEB_NUM_MAP.find(_atom->getType());
        // 常用原子邻接键级表中没有的原子不加氢
        if (ELEMENT_COMMON_NEB_NUM_MAP.end() == it) { return; }
        int order = get_atom_order(_atom->getId());
        int numHs = it->second - order;
        // 对电荷的特别处理
        int charge = _atom->getCharge();
        if (0 != charge) {
            switch (_atom->getType()) {
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
//        qDebug() << "numHs=" << numHs;
        if (numHs < 1) { return; }
        while (numHs--) {
            auto h = addAtom(ElementType::H);
            auto bond = addBond(_atom, h);
        }
    });
    setStartAddingHydrogens(false);
}

void JMol::loopCurrentAtomPtrVec(std::function<void(std::shared_ptr<JAtom>)> _func) {
    std::vector<std::shared_ptr<JAtom>> currentAtomPtr;
    for (auto &[aid, atom]:atomMap) {
        currentAtomPtr.push_back(atom);
    }
    for (auto &atom:currentAtomPtr) {
        if (atom) { _func(atom); }
    }
}

void JMol::setStartAddingHydrogens(bool startAddingHydrogens) {
    JMol::startAddingHydrogens = startAddingHydrogens;
}

void JMol::tryExpand() {

}

bool JMol::IsValidWritableFormat(const std::string &_suffix) {
    return FORMAT_WRITE_WHITE_LIST.end() != FORMAT_WRITE_WHITE_LIST.find(_suffix);
}


