#include <boost/test/unit_test.hpp>
#include <openbabel/babelconfig.h>

#include <fstream>

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/chargemodel.h>
#include <openbabel/obutil.h>
#include <openbabel/atom.h>
#include <openbabel/obiter.h>
#include <cstdlib>

using namespace std;
using namespace OpenBabel;

void GenerateGasteigerCharges(const std::string&amolecules_file,
                              const std::string&aresults_file,
                              const std::string&adipole_file);

BOOST_AUTO_TEST_CASE(charge_gasteiger)
{
  string aresults_file = TEST_SAMPLES_PATH + std::string("files/charge-gasteiger.txt");
  string adipole_file = TEST_SAMPLES_PATH + std::string("files/dipole-gasteiger.txt");
  string amolecules_file = TEST_SAMPLES_PATH + std::string("files/forcefield.sdf");
  // GenerateGasteigerCharges(amolecules_file, aresults_file, adipole_file);
  std::ifstream mifs;
  BOOST_ASSERT(SafeOpen(mifs, amolecules_file.c_str()));

  std::ifstream rifs;
  BOOST_ASSERT(SafeOpen(rifs, aresults_file.c_str()));

  std::ifstream difs;
  BOOST_ASSERT(SafeOpen(difs, adipole_file.c_str()));

  char buffer[BUFF_SIZE];
  vector<string> vs;
  OBMol mol;
  OBConversion conv(&mifs, &cerr);
  unsigned int currentTest = 0;
  vector3 dipoleMoment, result;
  
  std::vector<double> partialCharges;
  OBChargeModel *pCM;

  BOOST_ASSERT(conv.SetInAndOutFormats("SDF","SDF"));
      
  pCM = OBChargeModel::FindType("gasteiger");
  BOOST_ASSERT(pCM);

    while(mifs)
      {
        mol.Clear();
        conv.Read(&mol);
        if (mol.Empty())
          continue;
        BOOST_ASSERT(difs.getline(buffer,BUFF_SIZE));

        BOOST_ASSERT(pCM->ComputeCharges(mol));
        partialCharges = pCM->GetPartialCharges();

        // compare the calculated energy to our reference data
        tokenize(vs, buffer);
        BOOST_REQUIRE_GE(vs.size(), 3);

        dipoleMoment.SetX(atof(vs[0].c_str()));
        dipoleMoment.SetY(atof(vs[1].c_str()));
        dipoleMoment.SetZ(atof(vs[2].c_str()));
        result = pCM->GetDipoleMoment(mol) - dipoleMoment;
        BOOST_REQUIRE_LE(fabs(result.length_2()), 1.0e-4);

        FOR_ATOMS_OF_MOL(atom, mol) {
          BOOST_ASSERT(rifs.getline(buffer,BUFF_SIZE));
          BOOST_REQUIRE_CLOSE(atom->GetPartialCharge(), atof(buffer), 1.0e-3);
        }
      }
}

void GenerateGasteigerCharges(const std::string&amolecules_file,
                              const std::string&aresults_file,
                              const std::string&adipole_file)
{
  std::ifstream ifs;
  BOOST_ASSERT(SafeOpen(ifs, amolecules_file.c_str()));

  std::ofstream rofs;
  BOOST_ASSERT(SafeOpen(rofs, aresults_file.c_str()));

  std::ofstream dofs;
  BOOST_ASSERT(SafeOpen(dofs, adipole_file.c_str()));

  OBMol mol;
  OBConversion conv(&ifs, &cerr);
  char buffer[BUFF_SIZE];

  BOOST_ASSERT(conv.SetInAndOutFormats("SDF","SDF"));

  OBChargeModel *pCM = OBChargeModel::FindType("gasteiger");
  BOOST_ASSERT(pCM);

  std::vector<double> partialCharges;
  vector3 dipoleMoment;
  for (;ifs;)
    {
      mol.Clear();
      conv.Read(&mol);
      if (mol.Empty())
        continue;

      if (pCM->ComputeCharges(mol)) {
        partialCharges = pCM->GetPartialCharges();
      }

      // write out the dipole moment
      dipoleMoment = pCM->GetDipoleMoment(mol);
      sprintf(buffer, "%15.5f%15.5f%15.5f\n", dipoleMoment.x(), dipoleMoment.y(), dipoleMoment.z());
      dofs << buffer;
      
      // and write all the partial charges
      FOR_ATOMS_OF_MOL(atom, mol) {
        sprintf(buffer, "%15.5f\n", atom->GetPartialCharge());
        rofs << buffer;
      }
    }
  return;
}
