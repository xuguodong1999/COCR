#ifndef _MOL_HPP_
#define _MOL_HPP_

#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"
#include <unordered_map>
#include <opencv2/core/types.hpp>

/**
 * JMol 维护自己管理的原子和键的 id
 */
class JMol {
    size_t mAids, mBids;

    std::shared_ptr<JAtom> removeAtom(const size_t &_aid);

    std::shared_ptr<JBond> removeBond(const size_t &_bid);

    void addHs(const size_t &_aid);

    bool addRing(const std::string &_ringName, const size_t &_bid,
                 const int &_pos1, const int &_pos2);

protected:
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    std::unordered_map<size_t, cv::Point2f> atomPosMap;
public:
    const std::unordered_map<size_t, cv::Point2f> &getAtomPosMap() const;

public:
    const std::unordered_map<size_t, std::shared_ptr<JBond>> &getBondsMap() const;

    const std::unordered_map<size_t, std::shared_ptr<JAtom>> &getAtomsMap() const;

    JMol();

    void set(const std::string &_smiles);

    void randomize();

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

    void update2DCoordinates();

};

#endif//_MOL_HPP_