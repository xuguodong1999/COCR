#ifndef _MOL_UTIL_HPP_
#define _MOL_UTIL_HPP_

#include "mol.hpp"
#include "mol3d.hpp"

/**
  * 通过染色遍历寻找孤立的连通片，把孤立的连通片作为独立分子返回，不影响原有结构
  * @return independent molecules
  * TODO: 改造为原位dfs实现
  */
std::vector<std::shared_ptr<JMol>> split(const std::shared_ptr<JMol> &_mol);


class MolUtil {
public:
    virtual std::string getSMILES(const std::shared_ptr<JMol> &_mol) = 0;

    virtual std::vector<std::string> getForcefieldChoices() = 0;

    virtual std::vector<std::string> getFormatChoices() = 0;

    virtual std::shared_ptr<Mol3D> getCoord3D(const std::shared_ptr<JMol> &_mol) = 0;

    virtual std::string getFormat(const std::shared_ptr<JMol> &_mol, const std::string &_format) = 0;

    virtual std::shared_ptr<JMol> fromFormat(
            const std::string &_content, const std::string &_format) = 0;

    virtual std::vector<std::vector<size_t>> getLSSR(const std::shared_ptr<JMol> &_mol) = 0;

    virtual std::vector<std::vector<size_t>> getSSSR(const std::shared_ptr<JMol> &_mol) = 0;
};

#endif//_MOL_UTIL_HPP_
