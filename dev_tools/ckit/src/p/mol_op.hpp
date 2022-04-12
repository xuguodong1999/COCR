#pragma once

#include "chem/mol_holder.hpp"

class COCR_CHEM_EXPORT MolOp : public MolHolder {
    //<原子id，化合价or当前键级>
    std::unordered_map<size_t, frac> atomValenceMap;
    //<原子id，邻接的双键数>
    std::unordered_map<size_t, size_t> dbCounter;
    //<原子id，邻居原子们的id>
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborAtomsMap;
    //<原子id，键们的id>
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborBondsMap;
    //芳环
    std::vector<std::vector<std::unordered_set<size_t>>> aromaticRingAids, aromaticRingBids;

public:
    MolOp(std::shared_ptr<JMol> _mol);

    void randomize(const float &_addHydrogenProb = 0.1, bool _replaceBond = true,
                   bool _replaceAtom = true, bool _addAromaticRing = true,
                   bool _addCommonRing = true);

    /**
     * 为当前原子补全氢原子，修改化合价表
     * @param _aid 原子 id
     * @param _atomValenceMap 化合价表
     */
    void addHs(const size_t &_aid);

    bool markAsDouble(const size_t &_bid);

    /**
     * 把 _bid 两侧的基团挂到芳环上，删掉编号 _bid 的化学键
     * 涉及删除操作，会导致bondMap的迭代器失效
     * 当前支持的图元：呋喃、苯、萘、蒽、菲，全部二取代
     * @param _bid 化学键 id
     * @param _atomValenceMap 化合价参照表
     */
    void addAromaticRing(const size_t &_bid);


    /**
     * 把 _bid 两侧的基团挂到饱和环上，删掉编号 _bid 的化学键
     * 涉及删除操作，会导致bondMap的迭代器失效
     * 当前支持的图元：3-7 元环、六元单桥环
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

    void clear() override;

    /**
     * 获取手动添加的芳环
     * FIXME: 目前没有实现 or 对接 LSSR 和 SSSR 算法，不具备事后找环的能力
     * @param _retAid true: 返回原子索引，false: 返回键索引
     * @return 芳环列表，每一个元素是一些原子id的集合，表示一个最小环，比如菲占3个环
     */
    const std::vector<std::vector<std::unordered_set<size_t>>> &
    getAromaticRings(bool _retAid) const;

    /**
     * 全量更新化合价表
     */
    void updateAtomValenceMap();

    frac getValByAtomId(const size_t &_aid) const;

private:

    /**
     * 移除当前化学键
     * @param _bid 键id
     */
    void removeBond(const size_t &_bid);


    /**
     * 向 atomValenceMap[_aid] 添加 _valence
     * @param _aid 原子 id
     * @param _valence 待添加化合价
     */
    void addValence4Atom(const size_t &_aid, const frac &_valence);


    /**
     * 增量更新化合价表
     * @param _bonds 引起化合价变动的化学键数组
     */
    void updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds);

    /**
     * 全量更新原子邻居表，包括原子表和键表
     */
    void updateNeighborInfoMap();

    void updateDoubleBondCounter();
};