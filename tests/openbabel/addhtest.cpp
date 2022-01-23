///**********************************************************************
//addhtest.cpp - Test adding hydrogens
//
//Copyright (C) 2019 David R. Koes
//
//This file is part of the Open Babel project.
//For more information, see <http://openbabel.org/>
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation version 2 of the License.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//***********************************************************************/
//#include <boost/test/unit_test.hpp>
//#include <cstdlib>
//#include <openbabel/babelconfig.h>
//#include <openbabel/mol.h>
//#include <openbabel/obconversion.h>
//#include <openbabel/atom.h>
//#include <openbabel/bond.h>
//#include <openbabel/obiter.h>
//#include <openbabel/elements.h>
//#include <openbabel/generic.h>
//
//#include <cstdio>
//#include <iostream>
//#include <fstream>
//
//
//using namespace std;
//using namespace OpenBabel;
//
////add hydrogens and then perform some sanity checks
//void addh_check(OBMol *mol)
//{
//    unsigned len = mol->NumAtoms();
//    unsigned *vcnts = (unsigned*)alloca(len*sizeof(unsigned));
//    memset(vcnts, 0, sizeof(unsigned)*len);
//
//    //chemistry is weird, so can't rely on MaxBonds in general
//    //we recod the valence count before adding hydrogens
//    FOR_ATOMS_OF_MOL(atom, mol) {
//      int total_valence = atom->GetExplicitValence();
//      BOOST_REQUIRE_LE(atom->GetIndex(), len);
//      vcnts[atom->GetIndex()] = total_valence;
//    }
//    mol->AddHydrogens();
//    //check valences and bond lengths
//
//    FOR_ATOMS_OF_MOL(atom, mol) {
//      int total_valence = atom->GetTotalValence();
//      if(atom->GetIndex() >= len) {
//        BOOST_REQUIRE_EQUAL(total_valence, 1); //H
//        continue;
//      }
//      int maxval = max(OBElements::GetMaxBonds(atom->GetAtomicNum()), vcnts[atom->GetIndex()]);
//      BOOST_REQUIRE_LE(total_valence, maxval);
//      if(total_valence > maxval) {
//        cerr << "Hydrogens added when already greater than MaxBonds " << total_valence << " vs "
//            << OBElements::GetMaxBonds(atom->GetAtomicNum()) << " for atom "
//            << atom->GetId() << " " << OBElements::GetSymbol(atom->GetAtomicNum()) << "\n";
//        //exit(-1);
//      }
//    }
//
//    FOR_BONDS_OF_MOL(bond, mol) {
//      BOOST_REQUIRE_LE(bond->GetLength() , 5);
//      if(bond->GetLength() > 5) {
//        cerr << "Bond length between " << bond->GetBeginAtom()->GetId()
//            << " and " << bond->GetEndAtom()->GetId() << " is too long: "
//            << bond->GetLength() << "\n";
//        //exit(-1);
//      }
//    }
//}
//
//// 1
//// reads addh.in, which has a file name on each line,
//// reads molecule, adds hydrogens
//
//BOOST_AUTO_TEST_CASE(addhtest)
//{
//  string infile = TEST_SAMPLES_PATH;
//  infile += "files/addh.in";
//
//  ifstream ifs(infile.c_str());
//  BOOST_REQUIRE(ifs);
//
//
//  //gzip.in specifies what files to read and their canonical smiles
//  string line;
//  string filepath;
//  vector<string> correctResults; //read from file
//
//  OBConversion conv;
//  OBMol mol;
//  int max_test_num = 20, test_num = 0;
//
//  while(getline(ifs, line) && ++test_num <= max_test_num)
//  {
//    if(line.length() == 0)
//      continue; //ignore blank lines
//    if(line[0] != '#')
//    {
//      stringstream str(line);
//      string fname;
//      str >> fname;
//      OBFormat *format = conv.FormatFromExt(fname.c_str());
//      conv.SetInFormat(format);
//      filepath = TEST_SAMPLES_PATH;
//      filepath += "files/" + fname;
//
//      conv.ReadFile(&mol,filepath);
////      cerr << filepath << "\n";
//      addh_check(&mol);
//    }
//  }
//}
