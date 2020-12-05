#include "rdkit_util.hpp"
#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>

#ifdef WIN32
RDLogger rdErrorLog;
#endif
using namespace std;

string getStandardSMILES(const string &smiles, bool add_hydrogen, bool do_kekule) {
    string stdSMILES;
    try {
        shared_ptr<RDKit::ROMol> mol1(RDKit::SmilesToMol(smiles));
        stdSMILES = RDKit::MolToSmiles(
                *mol1, true, do_kekule, -1, true, false, add_hydrogen, false);
    } catch (exception &e) {
        cerr << "Exception in getStandardSMILES: " << e.what() << endl;
        exit(-1);
    }
    return move(stdSMILES);
}


