#include "obtest.h"
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace OpenBabel;

bool mdoMultiMoleculeFile(const std::string &filename)
{
  std::ifstream ifs;
  ifs.open(filename.c_str());
  BOOST_REQUIRE( ifs );

  OBMol mol;
  OBConversion conv(&ifs, &cerr);
  OBFormat *format = conv.FormatFromExt(filename.c_str());
  OBFormat *canSMI = conv.FindFormat("can");
  OBFormat *smi    = conv.FindFormat("smi");
  BOOST_REQUIRE(format);
  BOOST_REQUIRE(canSMI);
  BOOST_REQUIRE(smi);

  BOOST_REQUIRE(conv.SetInAndOutFormats(format, canSMI)); 

  string output, roundtrip; // first output string, then the roundtrip
  OBMol round2; // result of reading first output as canonical SMILES
  OBConversion conv2; // duplicate to prevent having to constantly change formats
  BOOST_REQUIRE(conv2.SetInAndOutFormats(smi, canSMI));

  bool result = true;
  conv.SetInStream(&ifs);
  int testCount = 0;
  int failed = 0;

  while (conv.Read(&mol)) {
    testCount++;
    mol.SetTitle("");
    output = conv.WriteString(&mol, true); // trim whitespace
    BOOST_REQUIRE(conv2.ReadString(&round2, output));
    round2.SetTitle("");
    roundtrip = conv2.WriteString(&round2, true);
    if (roundtrip != output) {
      failed++;
      result = false;
      bool ret = strcasecmp(output.c_str(), roundtrip.c_str()) != 0;
      if(!ret) {
          cerr << "Failed aromaticity: \n " << output << "\n";
      }else {
          cerr << "Failed roundtrip: \n  " << output << " -> \n  " << roundtrip << "\n";
      }
    }
    BOOST_REQUIRE_EQUAL( roundtrip, output);
  }
  return result;
}


BOOST_AUTO_TEST_CASE(canonconsistenttest)
{
      BOOST_REQUIRE( mdoMultiMoleculeFile(OBTestUtil::GetFilename("forcefield.sdf")) );
      BOOST_REQUIRE( mdoMultiMoleculeFile(OBTestUtil::GetFilename("filterset.sdf")) );
      BOOST_REQUIRE( mdoMultiMoleculeFile(OBTestUtil::GetFilename("cantest.sdf")) );
}

