#include <openbabel/babelconfig.h>
#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>
#include <openbabel/plugin.h>
#include <openbabel/babelconfig.h>
using namespace std;
using namespace OpenBabel;

int main() {
    //BABEL_DATADIR
//    _putenv_s(""",")

    _putenv("BABEL_DATADIR=C:/shared/openbabel3.1.1/bin/data");
    _putenv("BABEL_LIBDIR=C:/shared/openbabel3.1.1/bin");

    OBConversion conv;
    OBFormat *format_in = conv.FormatFromExt("smi");
    OBFormat *format_out = conv.FindFormat("pdb");
    const char *program_name = "obusage";
    if (!format_in || !format_out || !conv.SetInAndOutFormats(format_in, format_out)) {
        cerr << program_name << ": cannot read input/output format!" << endl;
        exit(-1);
    }


    const char *ff = "UFF";
    OBForceField::LoadAllPlugins();
    OBForceField *pFF = OBForceField::FindForceField(ff);
    if (!pFF) {
        cerr << program_name << ": could not find forcefield '" << ff << "'." << endl;
        exit(-3);
    }

    OBMol mol;
    mol.Clear();
    if (!conv.Read(&mol, &cin)) {
        exit(-1);
    }
    if (mol.Empty()) {
        exit(-2);
    }

    //mol.AddHydrogens(false, true); // hydrogens must be added before Setup(mol) is called

    pFF->SetLogFile(&cerr);
    pFF->SetLogLevel(OBFF_LOGLVL_LOW);

    //pFF->GenerateCoordinates();
    OBBuilder builder;
    builder.Build(mol);

    mol.AddHydrogens(false, true); // hydrogens must be added before Setup(mol) is called
    if (!pFF->Setup(mol)) {
        cerr << program_name << ": could not setup force field." << endl;
        exit(-1);
    }

    pFF->SteepestDescent(500, 1.0e-4);
    pFF->WeightedRotorSearch(250, 50);
    pFF->SteepestDescent(500, 1.0e-6);

    pFF->UpdateCoordinates(mol);
    //pFF->ValidateGradients();
    //pFF->SetLogLevel(OBFF_LOGLVL_HIGH);
    //pFF->Energy();


    //char FileOut[32];
    //sprintf(FileOut, "%s_obgen.pdb", basename.c_str());
    //ofs.open(FileOut);
    //conv.Write(&mol, &ofs);
    //ofs.close();
    conv.Write(&mol, &cout);
    return 0;
}