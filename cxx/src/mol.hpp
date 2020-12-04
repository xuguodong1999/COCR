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
    //芳环
    std::vector<std::vector<std::unordered_set<size_t>>> aromaticRingAids, aromaticRingBids;
    float fontSize;

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
     * 当前支持的图元：呋喃、苯、萘、蒽、菲，全部二取代
     * @param _bid 化学键 id
     * @param _atomValenceMap 化合价参照表
     */
    void addAromaticRing(const size_t &_bid);


    /**
     * 创建一个空环、向环中合理位置参入杂原子
     * @param _nodeNum 节点数
     * @param _singleLink 单键索引，从 1 开始连续增，建议遵循标准命名法
     * @param _doubleLink 双键索引，从 1 开始连续增，建议遵循标准命名法
     * @param _mixAtomicNumbers 参入哪些杂原子，通过重复某些元素控制杂原子比例分布
     * @return 和原有结构不冲突的环
     */
    std::pair<std::vector<std::shared_ptr<JAtom>>, std::vector<std::shared_ptr<JBond>>>
    makeCarbonRing(const size_t &_nodeNum,
                   const std::vector<std::pair<size_t, size_t>> &_singleLink,
                   const std::vector<std::pair<size_t, size_t>> &_doubleLink,
                   const std::vector<size_t> &_mixAtomicNumbers);


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

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

public:
    /**
     * "安全地"遍历当前所有化学键
     * 允许处理当前化学键时将其删除
     * 允许添加新的化学键，但是不会遍历到新的化学键
     * @param func 传入 bid，表示原子 id
     */
    void safeTraverseBonds(const std::function<void(const size_t &)> &func);

    /**
     * "安全地"遍历当前所有原子
     * 允许对原子进行增删
     * @param func 传入 aid，表示原子 id
     */
    void safeTraverseAtoms(const std::function<void(const size_t &)> &func);

    /**
     * 获取手动添加的芳环
     * FIXME: 目前没有实现 or 对接 LSSR 和 SSSR 算法，不具备事后找环的能力
     * @param _retAid true: 返回原子索引，false: 返回键索引
     * @return 芳环列表，每一个元素是一些原子id的集合，表示一个最小环，比如菲占3个环
     */
    const std::vector<std::vector<std::unordered_set<size_t>>> &
    getAromaticRings(bool _retAid) const;

    const std::unordered_map<size_t, std::shared_ptr<JBond>> &getBondsMap() const;

    const std::unordered_map<size_t, std::shared_ptr<JAtom>> &getAtomsMap() const;

    JMol();

    void set(const std::string &_smiles);

    void setAlkane(const std::string &_alkaneSMILES);

    std::string toSMILES(bool _addRandomStereo = true) const;

    void randomize(const float &_addHydrogenProb = 0.1, bool _replaceBond = true,
                   bool _replaceAtom = true, bool _addAromaticRing = true,
                   bool _addCommonRing = true);

    std::unordered_map<size_t, cv::Point2f> get2DCoordinates() const;

    std::unordered_map<size_t, cv::Point3f> get3DCoordinates(bool _addHs = false);

    /**
     * @return 特定绘图引擎下理想字体的边长，一般为化学键长度的 1/2
     */
    const float &getFontSize() const;
};

#endif//_MOL_HPP_