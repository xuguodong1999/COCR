/**********************************************************************
ffghemical.cpp - Test energy and gradients for Ghemical force field

This file is part of the Open Babel project.
For more information, see <http://openbabel.org/>

Some portions Copyright (C) 2008 Geoffrey R. Hutchison
 
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
#include <cstdlib>

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/forcefield.h>
#include <openbabel/obutil.h>
#include <fstream>
using namespace std;
using namespace OpenBabel;

void PGenerateEnergies(const string& emolecules_file, const string& eresults_file);

BOOST_AUTO_TEST_CASE(ffghemical)
{
  string eresults_file = TEST_SAMPLES_PATH + std::string("files/ghemicalresults.txt");
  string emolecules_file = TEST_SAMPLES_PATH + std::string("files/forcefield.sdf");

  // PGenerateEnergies(emolecules_file, eresults_file);

  std::ifstream mifs;
  BOOST_REQUIRE(SafeOpen(mifs, emolecules_file.c_str()));

  std::ifstream rifs;
  BOOST_REQUIRE(SafeOpen(rifs, eresults_file.c_str()));

  char buffer[BUFF_SIZE];
  vector<string> vs;
  OBMol mol;
  OBConversion conv(&mifs, &cerr);

  BOOST_REQUIRE(conv.SetInAndOutFormats("SDF","SDF"));
    
  OBForceField* pFF = OBForceField::FindForceField("Ghemical");
  BOOST_REQUIRE(pFF);

  pFF->SetLogFile(&cerr);
  pFF->SetLogLevel(OBFF_LOGLVL_NONE);

  double energy;
  while(mifs)
    {
      mol.Clear();
      conv.Read(&mol);
      if (mol.Empty())
        continue;
      BOOST_REQUIRE(rifs.getline(buffer,BUFF_SIZE));

      BOOST_REQUIRE(pFF->Setup(mol));

      // compare the calculated energy to our reference data
      energy = pFF->Energy(false);
      BOOST_REQUIRE_CLOSE(atof(buffer), energy, 1.0e-3);
      BOOST_REQUIRE(pFF->ValidateGradients());
    }
}

void PGenerateEnergies(const string& emolecules_file, const string& eresults_file)
{
  std::ifstream ifs;
  BOOST_REQUIRE(SafeOpen(ifs, emolecules_file.c_str()));

  std::ofstream ofs;
  BOOST_REQUIRE(SafeOpen(ofs, eresults_file.c_str()));

  OBMol mol;
  OBConversion conv(&ifs, &cerr);
  char buffer[BUFF_SIZE];

  BOOST_REQUIRE(conv.SetInAndOutFormats("SDF","SDF"));

  OBForceField* pFF = OBForceField::FindForceField("Ghemical");
  BOOST_REQUIRE(pFF);

  pFF->SetLogFile(&cerr);
  pFF->SetLogLevel(OBFF_LOGLVL_NONE);

  for (;ifs;)
    {
      mol.Clear();
      conv.Read(&mol);
      if (mol.Empty())
        continue;
      BOOST_REQUIRE(pFF->Setup(mol));
      // Don't compute gradients
      sprintf(buffer, "%15.5f\n", pFF->Energy(false));
      ofs << buffer;
    }
}
