#pragma once

#include "XGD/Chem/Config.h"
#include "XGD/Chem/Atom.h"
#include "XGD/Chem/Bond.h"

#include <memory>
#include <functional>
#include <unordered_map>

namespace data_deprecated {

    class JMol {
        // record incremental ids
        size_t mAids, mBids;
        //<atom id, atom>
        std::unordered_map<size_t, std::shared_ptr<Atom>> atomsMap;
        //<bond id, bond>
        std::unordered_map<size_t, std::shared_ptr<Bond>> bondsMap;
    public:
        JMol(const JMol &_mol);

        JMol(JMol &&_mol);

        JMol();

        /**
         * deep clone
         */
        std::shared_ptr<JMol> clone() const;

        void setAlkane(const std::string &_alkaneSMILES);

        std::shared_ptr<Atom> getAtomById(const size_t &_aid) const;

        std::shared_ptr<Bond> getBondById(const size_t &_bid) const;

        bool IsBondsEmpty() const;

        size_t bondsNum() const;

        size_t atomsNum() const;

        /**
         * allow delete or add bonds
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

        std::shared_ptr<Atom> addAtom(const size_t &_atomicNumber, const float &_x = -1, const float &_y = -1);

        std::shared_ptr<Atom> addAtom(const ElementType &_elementType);

        std::shared_ptr<Bond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                      const BondType &_bondType = BondType::SingleBond);

    };
}