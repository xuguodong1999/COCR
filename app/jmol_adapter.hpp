#ifndef _JMOL_ADAPTER_HPP_
#define _JMOL_ADAPTER_HPP_

#include "jmol.hpp"
#include <unordered_map>
#include <functional>
namespace OpenBabel {
    class OBMol;

    class OBAtom;

    class OBBond;
}
namespace xgd {
    class JMolAdapter : public JMol {
        OpenBabel::OBMol *obMol;
        std::unordered_map<decltype(id), OpenBabel::OBAtom *> atomIdMap;
        std::unordered_map<decltype(id), OpenBabel::OBBond *> bondIdMap;
        std::unordered_map<OpenBabel::OBAtom *, decltype(id)> atomIdMap2;
        std::unordered_map<OpenBabel::OBBond *, decltype(id)> bondIdMap2;
        bool is3DInfoLatest;

        bool runForcefield();

        void onMolUpdated();
        void syncAtoms(std::function<void(JAtom&, OpenBabel::OBAtom *)>_func);
    public:
        JMolAdapter();

        ~JMolAdapter();

        JMolAdapter(const JMolAdapter &_jMolAdapter);

        JMolAdapter(JMolAdapter &&_jMolAdapter);

        JMolAdapter &operator=(const JMolAdapter &) = delete;

        std::shared_ptr<JAtom> removeAtom(const size_t &_aid);

        std::shared_ptr<JBond> removeBond(const size_t &_bid);

        std::shared_ptr<JResidue> removeResidue(const size_t &_rid);

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JResidue> addResidue(
                const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z);

        std::shared_ptr<JBond> addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2);

        std::shared_ptr<JAtom> addAtom(const ElementType &_element, const float &_x = 0, const float &_y = 0);

        std::shared_ptr<JAtom> addAtom(
                const ElementType &_element, const float &_x, const float &_y, const float &_z);

        std::string writeAsPDB() ;

        std::string writeAsSMI() ;

        std::string writeAs(const std::string &_formatSuffix) ;

        void readAsPDB(const std::string &_pdbBuffer);

        void readAsSMI(const std::string &_pdbBuffer);

        void readAs(const std::string &_dataBuffer, const std::string &_formatSuffix);
    };
}


#endif//_JMOL_ADAPTER_HPP_
