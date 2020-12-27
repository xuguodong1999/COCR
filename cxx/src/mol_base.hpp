#ifndef _MOL_BASE_HPP_
#define _MOL_BASE_HPP_

//#include "atom.hpp"
//#include "bond.hpp"
//
//#include <unordered_map>
//#include <memory>
//#include <functional>
//
//class MolBase {
//    // 维护原子和键的自增 id
//    size_t mAids, mBids;
//    //<原子id，原子>
//    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
//    //<键id，键>
//    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
//protected:
//
//public:
//    MolBase();
//
//    /**
//     * "安全地"遍历当前所有化学键
//     * 允许处理当前化学键时将其删除
//     * 允许添加新的化学键，但是不会遍历到新的化学键
//     * @param func 传入 bid，表示原子 id
//     */
//    void safeTraverseBonds(const std::function<void(const size_t &)> &func);
//
//    /**
//     * "安全地"遍历当前所有原子
//     * 允许对原子进行增删
//     * @param func 传入 aid，表示原子 id
//     */
//    void safeTraverseAtoms(const std::function<void(const size_t &)> &func);
//};

#endif//_MOL_BASE_HPP_
