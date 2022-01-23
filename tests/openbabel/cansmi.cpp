 /**********************************************************************
 cansmi.cpp - Test Canonical SMILES generation -- write and then read

 This file is part of the Open Babel project.
 For more information, see <http://openbabel.org/>

 Copyright (C) 2001-2007 Geoffrey R. Hutchison

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 ***********************************************************************/
#include <boost/test/unit_test.hpp>
#include <openbabel/babelconfig.h>

#include <fstream>

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <cstdlib>

using namespace std;
using namespace OpenBabel;

BOOST_AUTO_TEST_CASE(cansmi)
{

  unsigned int currentMol = 0;
  OBMol mol, mol2;
  string buffer;
  string bsmilestypes_file = TEST_SAMPLES_PATH + std::string("/files/") + "nci.smi";
  
  std::ifstream mifs(bsmilestypes_file.c_str());
  BOOST_ASSERT(mifs);

  OBConversion conv(&mifs, &cerr);
  BOOST_ASSERT(conv.SetInAndOutFormats("SMI","CAN"));

    //read in molecules (as SMI), write as CANSMI, read in again
    while (getline(mifs, buffer))
      {
        mol.Clear();
        BOOST_ASSERT(conv.ReadString(&mol, buffer));
        if (mol.Empty())
          continue;

        buffer = conv.WriteString(&mol);

        mol2.Clear();
        BOOST_ASSERT(conv.ReadString(&mol2, buffer));
        BOOST_REQUIRE_EQUAL(mol.NumHvyAtoms(), mol2.NumHvyAtoms());
      }
}
