#ifndef _JMOL_ADAPTER_HPP_
#define _JMOL_ADAPTER_HPP_

#include "jmol.hpp"

namespace OpenBabel {
    class OBMol;
}
namespace xgd {
    class JMolAdapter : public JMol {
        OpenBabel::OBMol *obMol;
    public:
        JMolAdapter();
        ~JMolAdapter();
        JMolAdapter(const JMolAdapter&_jMolAdapter);
        JMolAdapter(JMolAdapter&&_jMolAdapter);
        JMolAdapter&operator=(const JMolAdapter&)=delete;
    };
}


#endif//_JMOL_ADAPTER_HPP_
