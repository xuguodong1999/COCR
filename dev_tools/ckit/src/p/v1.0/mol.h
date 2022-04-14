#pragma once

 #include "ckit/config.h"
#include "atom.h"
#include "bond.h"

#include <memory>
#include <functional>
#include <unordered_map>
namespace v1_0 {
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

        /**
         * 获取一个深拷贝
         * @return JMol 对象的深拷贝
         */
        std::shared_ptr<JMol> clone() const;

        void setAlkane(const std::string &_alkaneSMILES);

        std::shared_ptr<JAtom> getAtomById(const size_t &_aid) const;

        std::shared_ptr<JBond> getBondById(const size_t &_bid) const;

        bool IsBondsEmpty() const;

        size_t bondsNum() const;

        size_t atomsNum() const;

        /**
         * "安全地"遍历当前所有化学键
         * 允许处理当前化学键时将其删除
         * 允许添加新的化学键，但是不会遍历到新的化学键
         * @param func 传入 bid，表示原子 id
         */
        void safeTraverseBonds(const std::function<void(const size_t &)> &func) const;

        void safeTraverseBondsBreakIf(const std::function<bool(const size_t &)> &func) const;

        /**
         * "安全地"遍历当前所有原子
         * 允许对原子进行增删
         * @param func 传入 aid，表示原子 id
         */
        void safeTraverseAtoms(const std::function<void(const size_t &)> &func) const;

        void safeTraverseAtomsBreakIf(const std::function<bool(const size_t &)> &func) const;

        virtual void clear();

        void removeBond(const size_t &_bid);

        std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber, const float &_x = -1, const float &_y = -1);

        std::shared_ptr<JAtom> addAtom(const ElementType &_elementType);

        std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                       const BondType &_bondType = BondType::SingleBond);

    };
}