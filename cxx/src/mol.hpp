#ifndef _MOL_HPP_
#define _MOL_HPP_

#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"

#include <functional>
#include <unordered_map>

/**
 * JMol 维护自己管理的原子和键的 id
 * 辅助类一律采用持有 JMol 的智能指针的方式进行开发
 */
class JMol {
    // 维护原子和键的自增 id
    size_t mAids, mBids;
    //<原子id，原子>
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    //<键id，键>
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
public:
    JMol(const JMol &_mol);

    JMol(JMol &&_mol);

    JMol();

    void setAlkane(const std::string &_alkaneSMILES);


    std::shared_ptr<JAtom> getAtomById(const size_t &_aid) const;

    std::shared_ptr<JBond> getBondById(const size_t &_bid) const;

    bool emptyBonds() const;

    size_t bondsNum() const;

    size_t atomsNum() const;

    /**
     * "安全地"遍历当前所有化学键
     * 允许处理当前化学键时将其删除
     * 允许添加新的化学键，但是不会遍历到新的化学键
     * @param func 传入 bid，表示原子 id
     */
    void safeTraverseBonds(const std::function<void(const size_t &)> &func) const;

    /**
     * "安全地"遍历当前所有原子
     * 允许对原子进行增删
     * @param func 传入 aid，表示原子 id
     */
    void safeTraverseAtoms(const std::function<void(const size_t &)> &func) const;

    virtual void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

private:
};
#endif//_MOL_HPP_