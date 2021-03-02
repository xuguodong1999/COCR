#include "jmol_adapter.hpp"
#include <openbabel/mol.h>

xgd::JMolAdapter::JMolAdapter() {
    obMol = new OpenBabel::OBMol();
}

xgd::JMolAdapter::~JMolAdapter() {
    delete obMol;
    obMol = nullptr;
}

xgd::JMolAdapter::JMolAdapter(const xgd::JMolAdapter &_jMolAdapter) {
    obMol = new OpenBabel::OBMol(*(_jMolAdapter.obMol));
}

xgd::JMolAdapter::JMolAdapter(xgd::JMolAdapter &&_jMolAdapter) {
    obMol = _jMolAdapter.obMol;
}
