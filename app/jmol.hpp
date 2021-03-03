#ifndef _XGD_JMOL_HPP_
#define _XGD_JMOL_HPP_

#include "jatom.hpp"
#include "jbond.hpp"
#include "jresidue.hpp"
#include <unordered_set>
#include <vector>
#include <memory>

namespace xgd {
    class JMol {
    protected:
        size_t id;
        std::vector<std::shared_ptr<JAtom>> atomVec;
        std::vector<std::shared_ptr<JBond>> bondVec;
        std::vector<std::shared_ptr<JResidue>> residueVec;
        inline static std::unordered_set<std::string> sAvailableOutputFormat, sAvailableInputFormat;
    public:

        void setId(const size_t &_id);

        size_t getId();

        virtual std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        virtual std::shared_ptr<JAtom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z);

        virtual std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2);

        std::shared_ptr<JBond> addBond(const size_t &_aid1, const size_t &_aid2);

        virtual std::shared_ptr<JResidue>
        addResidue(const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);

        virtual std::shared_ptr<JResidue>
        addResidue(const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z);

        std::shared_ptr<JAtom> getAtom(const size_t &_aid);

        std::shared_ptr<JBond> getBond(const size_t &_bid);

        std::shared_ptr<JResidue> getResidue(const size_t &_rid);

        virtual std::shared_ptr<JAtom> removeAtom(const size_t &_aid);

        virtual std::shared_ptr<JBond> removeBond(const size_t &_bid);

        virtual std::shared_ptr<JResidue> removeResidue(const size_t &_rid);

        virtual std::string writeAsPDB()  = 0;

        virtual std::string writeAsSMI()  = 0;

        virtual std::string writeAs(const std::string &_formatSuffix)  = 0;

        virtual void readAsPDB(const std::string &_pdbBuffer) = 0;

        virtual void readAsSMI(const std::string &_pdbBuffer) = 0;

        virtual void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) = 0;
    };
}
#endif//_XGD_JMOL_HPP_
