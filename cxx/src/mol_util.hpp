#ifndef _MOL_UTIL_HPP_
#define _MOL_UTIL_HPP_

#include "mol.hpp"

/**
  * 通过染色遍历寻找孤立的连通片，把孤立的连通片作为独立分子返回，不影响原有结构
  * @return independent molecules
  * TODO: 改造为原位dfs实现
  */
std::vector<std::shared_ptr<JMol>> split(const JMol &_mol);
std::string getSMILES(const JMol &_mol);
#endif//_MOL_UTIL_HPP_
