#ifndef _MOL_HPP_
#define _MOL_HPP_

#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"

#include <opencv2/core/types.hpp>

#include <functional>
#include <unordered_map>

/**
 * JMol 维护自己管理的原子和键的 id
 */
class JMol {
    size_t mAids, mBids;
    std::unordered_map<size_t, frac> atomValenceMap;

    /**
     * 为当前原子补全氢原子，修改化合价表
     * @param _aid 原子 id
     * @param _atomValenceMap 化合价表
     */
    void addHs(const size_t &_aid);

    void removeBond(const size_t &_bid);

    /**
     * 把_bid两侧的基团挂到芳环上
     * 涉及删除操作，会导致bondMap的迭代器失效
     * @param _bid 化学键 id
     * @param _atomValenceMap 化合价参照表
     */
    void addAromaticRing(const size_t &_bid);

    void addCommonRing(const size_t &_bid);

    /**
     * 向原子_aid添加官能团，要求原子为碳且入度为1
     * @param _aid
     * @param _atomValenceMap
     */
    void addGroup(const size_t &_aid);

    void addValence4Atom(const size_t &_aid, const frac &_valence);

    // 全量更新化合价表
    void updateAtomValenceMap();

    // 增量更新化合价表
    void updateAtomValenceMap(const std::vector<size_t> &_bids);

    void updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds);

    /**
     * "安全地"遍历当前所有化学键
     * 允许处理当前化学键时将其删除
     * 允许添加新的化学键，但是不会遍历到新的化学键
     * @param func 传入 bid，表示原子 id
     */
    void safeTraverseBonds(std::function<void(const size_t&)> func);

protected:
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    std::unordered_map<size_t, cv::Point2f> atomPosMap;
public:
    const std::unordered_map<size_t, cv::Point2f> &getAtomPosMap() const;

    const std::unordered_map<size_t, std::shared_ptr<JBond>> &getBondsMap() const;

    const std::unordered_map<size_t, std::shared_ptr<JAtom>> &getAtomsMap() const;

    JMol();

    void set(const std::string &_smiles);

    /**
     * @return 特定绘图引擎下理想字体的边长，一般为化学键长度的 1/2
     */
    float getFontSize() const;

    std::string toSMILES(bool _addRandomStereo = true) const;

    void randomize(const float &_addHydrogenProb = 0.1);

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

    void update2DCoordinates();
};

#endif//_MOL_HPP_