#ifndef _MOL_GRAPH_HPP_
#define _MOL_GRAPH_HPP_
#include "mol_base.hpp"
#include <memory>
#include <vector>
/**
 * 不添加辅助数据结构完成图算法
 * 考虑到代码冗余，这里不和烷烃计数器共享图结构
 * 需求列表
 * 1、基于连通性的分拆
 * 2、LSSR、SSSR
 * 3、平面环的法向量（实现为Mol2D和MolGraph的双重继承）
 */
class MolGraph: virtual public MolBase{
public:
    /**
     * 根据图的连通性分解当前分子图
     * @return 连通的若干碎片
     */
    std::vector<std::shared_ptr<MolBase>>split()const;

};
#endif//_MOL_GRAPH_HPP_
