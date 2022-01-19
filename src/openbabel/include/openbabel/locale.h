/**********************************************************************
locale.h - Handle internal numeric locale issues -- parse data in "C"

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
#pragma once

#include <openbabel/babelconfig.h>
#include <locale>

namespace OpenBabel
{
  class OBLocalePrivate;

  // more detailed descriptions and documentation in locale.cpp
  //! \brief Handle the locale for numeric data parsing
  class OB_EXPORT OBLocale {
  public:

    OBLocale();
    ~OBLocale();

    void SetLocale();
    void RestoreLocale();

  protected:
    OBLocalePrivate* d;
  };

  //global definitions
  //! Global OBLocale for setting and restoring locale information
  OB_EXPORT extern  OBLocale   obLocale;

} // namespace OpenBabel

//! \file locale.h
//! \brief Handle internal numeric locale issues -- parse data in "C"