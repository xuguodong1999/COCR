#ifndef _RDKIT_UTIL_HPP_
#define _RDKIT_UTIL_HPP_

#include "mol_util.hpp"
#include <string>
#include <vector>

class MolUtilOpenBabelImpl : public MolUtil {
public:
    std::string getSMILES(const JMol &_jMol);

    std::vector<std::string> getForcefieldChoices();

    std::vector<std::string> getFormatChoices();

    bool getCoord3D(Mol3D &_mol3d);

    std::string getFormat(const JMol &_mol, const std::string &_format);

    std::shared_ptr<JMol> fromFormat(
            const std::string &_content, const std::string &_format);

    std::vector<std::vector<size_t>> getLSSR(const JMol &_mol);

    std::vector<std::vector<size_t>> getSSSR(const JMol &_mol);
};

#endif