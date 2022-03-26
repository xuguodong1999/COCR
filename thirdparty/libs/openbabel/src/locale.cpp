/**********************************************************************
locale.cpp - Handle internal numeric locale issues -- parse data in "C"

Copyright (C) 2008 by Geoffrey R. Hutchison

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
#include <openbabel/locale.h>
#include <locale>
namespace OpenBabel
{
  class OBLocalePrivate {
  public:
    unsigned int counter; // Reference counter -- ensures balance in SetLocale/RestoreLocale calls
    std::locale oldLocale;
    OBLocalePrivate(): counter(0)
    {
      std::locale::global(std::locale("C"));
    }
  }; // class definition for OBLocalePrivate

  /** \class OBLocale locale.h <openbabel/locale.h>
   *
   * Many users will utilize Open Babel and tools built on top of the library
   * in non-English locales. In particular, the numeric locale (LC_NUMERIC)
   * is used to determine the parsing of numeric data in files, reference data,
   * etc.
   *
   * The OBLocale class, and its global variable obLocale handle both internal
   * parsing of data through the ANSI-C numeric locale and can be used for
   * external use as well.
   *
   * In particular, where available, OBLocale will use the enhanced uselocale()
   * interface, which sets the locale for a particular thread, rather
   * for the entire process. (This is available on Linux, Mac OS X,
   * and other platforms.)
   *
   * \code
   * obLocale.SetLocale(); // set the numeric locale before reading data
   * ... // your data processing here
   * obLocale.RestoreLocale(); // restore the numeric locale
   * \endcode
   *
   * To prevent errors, OBLocale will handle reference counting.
   * If nested function calls all set the locale, only the first call
   * to SetLocale() and the last call to RestoreLocale() will do any work.
   **/

  OBLocale::OBLocale(): d(std::make_shared<OBLocalePrivate>()) {}

  void OBLocale::SetLocale()
  {
    if (d->counter == 0) {
      // Set the locale for number parsing to avoid locale issues: PR#1785463
      // Extended per-thread interface
      d->oldLocale = std::locale();
      std::locale::global(std::locale("C"));
    }

    ++d->counter;
  }

  void OBLocale::RestoreLocale()
  {
    --d->counter;
    if(d->counter == 0) {
      // return the locale to the original one
        std::locale::global(d->oldLocale);
    }
  }

  //global definitions
  // Global OBLocale for setting and restoring locale information
  OBLocale   obLocale;

} // namespace OpenBabel

//! \file locale.cpp
//! \brief Handle internal numeric locale issues -- parse data in "C"