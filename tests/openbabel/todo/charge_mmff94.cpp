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

void GenerateMMFF94Charges(const string& results_file, const string& dipole_file, const string& molecules_file);

BOOST_AUTO_TEST_CASE(charge_mmff94)
{
  string results_file = "files/charge-mmff94.txt";
  string dipole_file = "files/dipole-mmff94.txt";
  string molecules_file = "files/forcefield.sdf";
  //  GenerateMMFF94Charges(results_file, dipole_file, molecules_file);
  std::ifstream mifs;
  BOOST_ASSERT(SafeOpen(mifs, molecules_file.c_str()));

  std::ifstream rifs;
  BOOST_ASSERT(SafeOpen(rifs, results_file.c_str()));

  std::ifstream difs;
  BOOST_ASSERT(SafeOpen(difs, dipole_file.c_str()));

  char buffer[BUFF_SIZE];
  vector<string> vs;
  OBMol mol;
  OBConversion conv(&mifs, &cerr);
  vector3 dipoleMoment, result;
  
  std::vector<double> partialCharges;
  OBChargeModel *pCM;

  BOOST_ASSERT(conv.SetInAndOutFormats("SDF","SDF"));
      
    pCM = OBChargeModel::FindType("mmff94");
    BOOST_ASSERT(pCM);

    while(mifs)
      {
        mol.Clear();
        conv.Read(&mol);
        if (mol.Empty())
          continue; //dkoes - this should totally be break
        BOOST_ASSERT(difs.getline(buffer,BUFF_SIZE));

        BOOST_ASSERT(pCM->ComputeCharges(mol));
        partialCharges = pCM->GetPartialCharges();

        // compare the calculated energy to our reference data
        tokenize(vs, buffer);
        BOOST_REQUIRE_LE(vs.size() , 3);

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

void GenerateMMFF94Charges(const string& results_file, const string& dipole_file, const string& molecules_file)
{
  std::ifstream ifs;
  if (!SafeOpen(ifs, molecules_file.c_str()))
    return;

  std::ofstream rofs;
  if (!SafeOpen(rofs, results_file.c_str()))
    return;

  std::ofstream dofs;
  if (!SafeOpen(dofs, dipole_file.c_str()))
    return;

  OBMol mol;
  OBConversion conv(&ifs, &cerr);
  char buffer[BUFF_SIZE];
  
  if(! conv.SetInAndOutFormats("SDF","SDF"))
    {
      cerr << "SDF format is not loaded" << endl;
      return;
    }

  OBChargeModel *pCM = OBChargeModel::FindType("mmff94");

  if (pCM == nullptr) {
    cerr << "Cannot load charge model!" << endl;
    return;
  }

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
