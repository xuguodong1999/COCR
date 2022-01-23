/**********************************************************************
format.cpp - Unit tests for Open Babel OBFormat class

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
#include <openbabel/babelconfig.h>
#include <openbabel/mol.h>
#include <cstdlib>
#include <openbabel/obconversion.h>

#include <cstdio>
#include <iostream>

using namespace std;
using namespace OpenBabel;

BOOST_AUTO_TEST_CASE(format)
{
  int defaultchoice = 1;
  
  int choice = defaultchoice;

  if (argc > 1) {
    if(sscanf(argv[1], "%d", &choice) != 1) {
      printf("Couldn't parse that input as a number\n");
      return -1;
    }
  }

  // Define location of file formats for testing
  #ifdef FORMATDIR
    char env[BUFF_SIZE];
    snprintf(env, BUFF_SIZE, "BABEL_LIBDIR=%s", FORMATDIR);
    putenv(env);
  #endif

  cout << "# Unit tests for OBFormat \n";

  // the number of tests for "prove"
  cout << "1..4\n";

  cout << "ok 1\n"; // for loading tests

  OBConversion obConversion;
  obConversion.SetInAndOutFormats("smi", "mdl");
  cout << "ok 2\n";

  OBFormat *inFormat = obConversion.GetInFormat();
  if (inFormat)
    cout << "ok 3\n";
  else
    cout << "not ok 3\n";

  OBFormat *outFormat = obConversion.GetOutFormat();
  if (outFormat)
    cout << "ok 4\n";
  else
    cout << "not ok 4\n";

  return(0);
}

