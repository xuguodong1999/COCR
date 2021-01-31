#ifndef _MOL_UTIL_HPP_
#define _MOL_UTIL_HPP_

#include "mol.hpp"
#include "mol3d.hpp"


class MolUtil {
public:
    /**
      * 通过染色遍历寻找孤立的连通片，把孤立的连通片作为独立分子返回，不影响原有结构
      * @return independent molecules
      * TODO: 改造为原位dfs实现
      */
    std::vector<std::shared_ptr<JMol>> split(const JMol &_mol);

    virtual std::string getSMILES(const JMol &_mol) = 0;

    virtual std::vector<std::string> getForcefieldChoices() = 0;

    virtual std::vector<std::string> getFormatChoices() = 0;

    virtual bool getCoord3D(Mol3D &_mol3d) = 0;

    virtual std::string getFormat(const JMol &_mol, const std::string &_format) = 0;

    virtual std::shared_ptr<JMol> fromFormat(
            const std::string &_content, const std::string &_format) = 0;

    virtual std::vector<std::vector<size_t>> getLSSR(const JMol &_mol) = 0;

    virtual std::vector<std::vector<size_t>> getSSSR(const JMol &_mol) = 0;
};

#endif//_MOL_UTIL_HPP_
