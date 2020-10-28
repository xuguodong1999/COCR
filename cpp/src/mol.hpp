#ifndef _MOLE_HPP_
#define _MOLE_HPP_

#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"
#include <map>
#include <opencv2/core/types.hpp>

class JMol : public IdInterface {
protected:
    std::map<size_t, std::shared_ptr<JBond>> bondsMap;
public:
    const std::map<size_t, std::shared_ptr<JBond>> &getBondsMap() const;

    const std::map<size_t, std::shared_ptr<JAtom>> &getAtomsMap() const;

protected:
    std::map<size_t, std::shared_ptr<JAtom>> atomsMap;
    std::map<size_t, cv::Point2f> atomsPosMap;

public:
    JMol(const std::string &_smiles);

    JMol() {}

    void set(const std::string &_smiles);

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber = static_cast<size_t>(ElementType::C));

    std::shared_ptr<JBond>
    addBond(const size_t &_atomFromId, const size_t &_atomToId);

    std::shared_ptr<JBond>
    addBond(const size_t &_atomFromId, const size_t &_atomToId, const JBondType &_bondType);

    void update2DCoordinates();

};

#endif//_MOLE_HPP_