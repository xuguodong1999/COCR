#include "ckit/mol_util.h"
#include <openbabel/plugin.h>

bool MolUtil::IsValidWritableFormat(const std::string &suffix) {
    return false;
}

std::vector<std::string> MolUtil::GetWritableFormats() {
    std::vector<std::string> result;
    OpenBabel::OBPlugin::ListAsVector("formats", nullptr, result);
    return result;
}
