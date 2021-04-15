#include <iostream>
#include <sstream>
#include <algorithm>

#include <openbabel/babelconfig.h>
#include <openbabel/base.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/obconversion.h>
#include <openbabel/forcefield.h>
#include <openbabel/builder.h>

using namespace std;
using namespace OpenBabel;

void exit_and_log(int index) {
    std::cerr << index << std::endl;
    exit(index);
}

int main(int argc, char **argv) {

    // Find Input filetype
    OBConversion conv;
    OBFormat *format_in = conv.FormatFromExt("smi");
    OBFormat *format_out = conv.FindFormat("pdb");

    if (!format_in || !format_out || !conv.SetInAndOutFormats(format_in, format_out)) {
        exit_and_log(-1);
    }

    string data = "CC(=O)C";
    istringstream ifs(data);
    ostringstream ofs;

    OBMol *mol = new OBMol();
    mol->Clear();
//    {
//        if (!conv.Read(mol, &ifs)) {
//            exit_and_log(-2);
//        }
//    }

    {
        OBAtom atom;
        atom.SetAtomicNum(6);
        atom.SetId(0);
        mol->AddAtom(atom, false);

        atom.SetAtomicNum(6);
        atom.SetId(1);
        mol->AddAtom(atom, false);

        atom.SetAtomicNum(6);
        atom.SetId(2);
        mol->AddAtom(atom, false);

        atom.SetAtomicNum(8);
        atom.SetId(3);
        mol->AddAtom(atom, false);

        OBBond bond;
        bond.SetBondOrder(1);
        bond.SetBegin(mol->GetAtomById(0));
        bond.SetEnd(mol->GetAtomById(1));
        mol->AddBond(bond);

        bond.SetBondOrder(1);
        bond.SetBegin(mol->GetAtomById(0));
        bond.SetEnd(mol->GetAtomById(2));
        mol->AddBond(bond);

        bond.SetBondOrder(2);
        bond.SetBegin(mol->GetAtomById(0));
        bond.SetEnd(mol->GetAtomById(3));
        mol->AddBond(bond);

        /// Hydrogen atom
        atom.SetAtomicNum(1);
        atom.SetId(4);
        mol->AddAtom(atom, false);

        atom.SetAtomicNum(1);
        atom.SetId(5);
        mol->AddAtom(atom, false);

        atom.SetAtomicNum(1);
        atom.SetId(6);
        mol->AddAtom(atom, false);

        /// Hydrogen bond
        bond.SetBondOrder(1);
        bond.SetBegin(mol->GetAtomById(1));
        bond.SetEnd(mol->GetAtomById(4));
        mol->AddBond(bond);

        bond.SetBondOrder(1);
        bond.SetBegin(mol->GetAtomById(1));
        bond.SetEnd(mol->GetAtomById(5));
        mol->AddBond(bond);

        bond.SetBondOrder(1);
        bond.SetBegin(mol->GetAtomById(1));
        bond.SetEnd(mol->GetAtomById(6));
        mol->AddBond(bond);
    }

    if (mol->Empty()) {
        exit_and_log(-3);
    }

    OBForceField *pFF = OBForceField::FindForceField("uff");
    if (!pFF) {
        exit_and_log(-4);
    }
//    mol->SetHydrogensAdded(false);
//    mol->AddHydrogens(false, true); // hydrogens must be added before Setup(mol) is called

    pFF->SetLogLevel(OBFF_LOGLVL_NONE);

    //pFF->GenerateCoordinates();
    OBBuilder builder;
    builder.Build(*mol);

//    mol->AddHydrogens(false, true); // hydrogens must be added before Setup(mol) is called
    if (!pFF->Setup(*mol)) {
        exit_and_log(-5);
    }

    pFF->SteepestDescent(500, 1.0e-4);
    pFF->WeightedRotorSearch(250, 50);
    pFF->SteepestDescent(500, 1.0e-6);

    pFF->UpdateCoordinates(*mol);
    //pFF->ValidateGradients();
    //pFF->SetLogLevel(OBFF_LOGLVL_HIGH);
    //pFF->Energy();


    //char FileOut[32];
    //sprintf(FileOut, "%s_obgen.pdb", basename.c_str());
    //ofs.open(FileOut);
    //conv.Write(mol, &ofs);
    //ofs.close();
    conv.Write(mol, &cout);

    return 0;
}