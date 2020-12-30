#ifndef _RDKIT_UTIL_HPP_
#define _RDKIT_UTIL_HPP_

#include "mol_util.hpp"

#include <string>
#include <vector>

//std::string getStandardSMILES(const std::string &smiles,
//                              bool add_hydrogen = false, bool do_kekule = false);


class MolUtilOpenBabelImpl : public MolUtil {
public:
    std::string getSMILES(const std::shared_ptr<JMol> &_mol);

    std::vector<std::string> getForcefieldChoices();

    std::vector<std::string> getFormatChoices();

    std::shared_ptr<Mol3D> getCoord3D(const std::shared_ptr<JMol> &_mol);

    std::string getFormat(const std::shared_ptr<JMol> &_mol, const std::string &_format);

    std::shared_ptr<JMol> fromFormat(
            const std::string &_content, const std::string &_format);

    std::vector<std::vector<size_t>> getLSSR(const std::shared_ptr<JMol> &_mol);

    std::vector<std::vector<size_t>> getSSSR(const std::shared_ptr<JMol> &_mol);
};

#endif//_RDKIT_UTIL_HPP_