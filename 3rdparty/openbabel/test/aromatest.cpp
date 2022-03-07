
/**********************************************************************
aromatest.cpp - Test Open Babel aromaticity perception

Copyright (C) 2005-2006 Geoffrey R. Hutchison
 
This file is part of the Open Babel project.
For more information, see <http://openbabel.org/>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/
#include <boost/test/unit_test.hpp>
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/obiter.h>
#include <openbabel/obconversion.h>
#include <openbabel/elements.h>

#include <cstdio>
#include <iostream>
#include <fstream>

using namespace std;
using namespace OpenBabel;

// Molecules aromatized in error (in the course of porting from SMARTS patterns)
void NegativeTestCases(int &molCount, unsigned int &testCount)
{
  // Check that not every atom is aromatic (i.e. negative test cases)
  const char* smiles[] = { "c1ccc2[N+]=c3ccccc3=Nc2c1", // N radical found in eMolecules
                           "N1S[SH+]C=C1",
                           "S1C=[NH+]=[NH+]=C1",
                           "C1(N23)=CC=CC2=CC=CC3=CC=C1", // pyrido[2,1,6-de]quinolizine - no atom is Daylight aromatic
                           nullptr };
  OBMol mol;
  OBConversion conv;
  conv.SetInFormat("smi");

  for (int i = 0; smiles[i]; ++i) {
    mol.Clear();
    conv.ReadString(&mol, smiles[i]);
    molCount++;
    bool found_non_aromatic = false;
    FOR_ATOMS_OF_MOL(atom, mol) {
      if (atom->GetAtomicNum() == OBElements::Hydrogen)
        continue;
      if (!atom->IsAromatic()) {
        found_non_aromatic = true;
        break;
      }
    }

    if(!found_non_aromatic) {
        cerr << "not ok " << ++testCount << " # all atoms are aromatic in molecule " << molCount << " "
             << mol.GetTitle() << "\n";
    } else {
        ++testCount;
    }
    BOOST_REQUIRE(found_non_aromatic);
  }
}

BOOST_AUTO_TEST_CASE(aromatest)
{
  string filename = TEST_SAMPLES_PATH;
  filename += "files/aromatics.smi";

  ifstream ifs(filename.c_str());
  BOOST_REQUIRE(ifs);
  
  OBConversion conv(&ifs, &cerr);
  OBFormat* pFormat;
  
  pFormat = conv.FormatFromExt(filename);
  BOOST_REQUIRE(pFormat);
  
  // Finally, we can do some work!
  OBMol mol;
  
  unsigned int testCount = 0;

  BOOST_REQUIRE(conv.SetInAndOutFormats(pFormat, pFormat));
  int molCount = 0;
  while(ifs.peek() != EOF && ifs.good())
  {
      mol.Clear();
      conv.Read(&mol);
      molCount++;
      for (int N = 0; N < 2; ++N)
      {
          if (N == 0)
              mol.AddHydrogens();
          else
              mol.DeleteHydrogens();
          FOR_ATOMS_OF_MOL(atom, mol)
          {
              if (atom->GetAtomicNum() == OBElements::Hydrogen)
                  continue;
              ++testCount;
              if(!atom->IsAromatic()) {
                  cerr << "not ok " << ++testCount << " # atom isn't aromatic!\n"
                       << "# atom idx " << atom->GetIdx()
                       << " in molecule " << molCount << " "
                       << mol.GetTitle() << "\n";
              }
              BOOST_REQUIRE(atom->IsAromatic());
          }
      }
  } // while reading molecules
  NegativeTestCases(molCount, testCount);
}
