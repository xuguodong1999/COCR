#include "openbabel_util.hpp"

#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

//#include <GraphMol/GraphMol.h>
//#include <GraphMol/SmilesParse/SmilesParse.h>
//#include <GraphMol/SmilesParse/SmilesWrite.h>
//
//#ifdef WIN32
//RDLogger rdErrorLog;
//#endif
//using namespace std;
//
//string getStandardSMILES(const string &smiles, bool add_hydrogen, bool do_kekule) {
//    string stdSMILES;
//    try {
//        shared_ptr<RDKit::ROMol> mol1(RDKit::SmilesToMol(smiles));
//        stdSMILES = RDKit::MolToSmiles(
//                *mol1, true, do_kekule, -1, true, false, add_hydrogen, false);
//    } catch (exception &e) {
//        cerr << "Exception in getStandardSMILES: " << e.what() << endl;
//        exit(-1);
//    }
//    return move(stdSMILES);
//}


std::string getSMILES_openbabelImpl(const JMol &_mol) {
    return std::string();
}
