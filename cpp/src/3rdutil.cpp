#include "3rdutil.hpp"
#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/MolOps.h>

#ifdef _DEBUG
RDLogger rdErrorLog;
#endif

std::string getStandardSMILES(const string &smiles, bool add_hydrogen, bool do_kekule) {
    std::string stdSMILES;
    try {
        std::shared_ptr<RDKit::ROMol> mol1(RDKit::SmilesToMol(smiles));
        stdSMILES = RDKit::MolToSmiles(
                *mol1, true, do_kekule, -1, true, false, add_hydrogen, false);
    } catch (std::exception e) {
        std::cerr << "Exception in getStandardSMILES: " << e.what() << std::endl;
        exit(-1);
    }
    return std::move(stdSMILES);
}


