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
    // 维护原子和键的自增 id
    size_t mAids, mBids;
    //<原子id，原子>
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    //<键id，键>
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
    //<原子id，化合价or当前键级>
    std::unordered_map<size_t, frac> atomValenceMap;
    //<原子id，邻居原子们的id>
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborAtomsMap;
    //<原子id，键们的id>
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborBondsMap;


    /**
     * 为当前原子补全氢原子，修改化合价表
     * @param _aid 原子 id
     * @param _atomValenceMap 化合价表
     */
    void addHs(const size_t &_aid);

    /**
     * 移除当前化学键
     * @param _bid 键id
     */
    void removeBond(const size_t &_bid);

    /**
     * 把 _bid 两侧的基团挂到芳环上，删掉编号 _bid 的化学键
     * 涉及删除操作，会导致bondMap的迭代器失效
     * @param _bid 化学键 id
     * @param _atomValenceMap 化合价参照表
     */
    void addAromaticRing(const size_t &_bid);

    /**
     * 把 _bid 两侧的基团挂到饱和环上，删掉编号 _bid 的化学键
     * 涉及删除操作，会导致bondMap的迭代器失效
     * @param _bid 化学键 id
     * @param _atomValenceMap 化合价参照表
     */
    void addCommonRing(const size_t &_bid);

    /**
     * 向原子_aid添加官能团，要求原子为碳且入度为1
     * @param _aid
     * @param _atomValenceMap
     */
    void addGroup(const size_t &_aid);

    /**
     * 向 atomValenceMap[_aid] 添加 _valence
     * @param _aid 原子 id
     * @param _valence 待添加化合价
     */
    void addValence4Atom(const size_t &_aid, const frac &_valence);

    /**
     * 全量更新化合价表
     */
    void updateAtomValenceMap();

    /**
     * 增量更新化合价表
     * @param _bids 引起化合价变动的化学键id数组
     */
    void updateAtomValenceMap(const std::vector<size_t> &_bids);

    /**
     * 增量更新化合价表
     * @param _bonds 引起化合价变动的化学键数组
     */
    void updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds);

    /**
     * 全量更新原子邻居表，包括原子表和键表
     */
    void updateNeighborInfoMap();

    /**
     * "安全地"遍历当前所有化学键
     * 允许处理当前化学键时将其删除
     * 允许添加新的化学键，但是不会遍历到新的化学键
     * @param func 传入 bid，表示原子 id
     */
    void safeTraverseBonds(const std::function<void(const size_t &)> &func);

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

public:
    const std::unordered_map<size_t, std::shared_ptr<JBond>> &getBondsMap() const;

    const std::unordered_map<size_t, std::shared_ptr<JAtom>> &getAtomsMap() const;

    JMol();

    void set(const std::string &_smiles);

    std::string toSMILES(bool _addRandomStereo = true) const;

    void randomize(const float &_addHydrogenProb = 0.1, bool _replaceBond = true,
                   bool _replaceAtom = true, bool _addAromaticRing = true,
                   bool _addCommonRing=true);

    std::unordered_map<size_t, cv::Point2f> get2DCoordinates() const;

    /**
     * @return 特定绘图引擎下理想字体的边长，一般为化学键长度的 1/2
     */
    float getFontSize() const;
};

#endif//_MOL_HPP_