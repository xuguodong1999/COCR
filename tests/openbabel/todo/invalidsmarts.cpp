/**********************************************************************
invalid-smarts.cpp - Test SMARTS pattern parsing (rejecting invalid patterns)

This file is part of the Open Babel project.
For more information, see <http://openbabel.org/>

Some portions Copyright (C) 2005-2006 Geoffrey R. Hutchison
 
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
#include <openbabel/obutil.h>
#include <openbabel/oberror.h>
#include <openbabel/parsmart.h>

using namespace std;
using namespace OpenBabel;


BOOST_AUTO_TEST_CASE(invalidsmarts)
{
  string hinvalid_file = TEST_SAMPLES_PATH + std::string("/") + "files/invalid-smarts.txt";
  string hrandom1_file = TEST_SAMPLES_PATH + std::string("/") + "files/random";
  string hrandom2_file = TEST_SAMPLES_PATH + std::string("/") + "files/random2";
  string hrandom3_file = TEST_SAMPLES_PATH + std::string("/") + "files/random3";
  // make sure to kill off all error reporting
  obErrorLog.StopLogging();

  OBSmartsPattern smarts;
  string pattern, buffer;

  std::ifstream mifs;
  BOOST_ASSERT(SafeOpen(mifs, hinvalid_file.c_str()));

  while (getline(mifs, pattern))
    {
      // each line is a potential test pattern
      BOOST_ASSERT(smarts.Init(pattern));
    }

  mifs.close();
  mifs.clear();

  // random file#1
  BOOST_ASSERT(SafeOpen(mifs, hrandom1_file.c_str()));

  pattern.clear();
  while (getline(mifs, buffer))
    pattern += buffer;
  BOOST_ASSERT(smarts.Init(pattern));

  mifs.close();
  mifs.clear();

  // random2
  BOOST_ASSERT(smarts.Init(pattern));

  BOOST_ASSERT(SafeOpen(mifs, hrandom2_file.c_str()));

  pattern.clear();
  while (getline(mifs, buffer))
    pattern += buffer;
  BOOST_ASSERT(smarts.Init(pattern));

  mifs.close();
  mifs.clear();

  // random3
  BOOST_ASSERT(SafeOpen(mifs, hrandom3_file.c_str()));

  pattern.clear();
  while (getline(mifs, buffer))
    pattern += buffer;
  BOOST_ASSERT(smarts.Init(pattern));
}
