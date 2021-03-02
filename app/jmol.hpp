#ifndef _XGD_JMOL_HPP_
#define _XGD_JMOL_HPP_

#include "jatom.hpp"
#include "jbond.hpp"
#include "jresidue.hpp"

#include <vector>
#include <memory>

namespace xgd {
    class JMol {
    protected:
        std::vector<std::shared_ptr<JAtom>> atomVec;
        std::vector<std::shared_ptr<JBond>> bondVec;
        std::vector<std::shared_ptr<JResidue>> residueVec;
    public:

        std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x, const float &_y, const float &_z);

        std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2);

        std::shared_ptr<JBond> addBond(const size_t &_aid1, const size_t &_aid2);

        std::shared_ptr<JResidue>
        addResidue(const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JAtom> getAtom(const size_t &_aid);

        std::shared_ptr<JBond> getBond(const size_t &_bid);

        std::shared_ptr<JResidue> getResidue(const size_t &_rid);

        std::shared_ptr<JAtom> removeAtom(const size_t &_aid,bool _check=false);

        std::shared_ptr<JBond> removeBond(const size_t &_bid,bool _check=false);

        std::shared_ptr<JResidue> removeResidue(const size_t &_rid,bool _check=false);
    };
}
#endif//_XGD_JMOL_HPP_
