/**********************************************************************
Copyright (C) 1998-2001 by OpenEye Scientific Software, Inc.
Some portions Copyright (C) 2001-2006 by Geoffrey R. Hutchison
Some portions Copyright (C) 2004 by Chris Morley

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/
#include <openbabel/babelconfig.h>
#include <openbabel/obmolecformat.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/elements.h>
#include <openbabel/internalcoord.h>

#include <fmt/format.h>

using namespace std;
namespace OpenBabel
{

class FenskeZmatFormat : public OBMoleculeFormat
{
public:
    //Register this format type ID
    FenskeZmatFormat()
    {
        OBConversion::RegisterFormat("fh",this);
    }

    virtual const char* Description() //required
    {
        return
          "Fenske-Hall Z-Matrix format\n"
          "No comments yet\n";
    };

  virtual const char* SpecificationURL()
  {return "";}; //optional

    //Flags() can return be any the following combined by | or be omitted if none apply
    // NOTREADABLE  READONEONLY  NOTWRITABLE  WRITEONEONLY
    virtual unsigned int Flags()
    {
        return NOTREADABLE | WRITEONEONLY;
    };

    ////////////////////////////////////////////////////
    /// The "API" interface functions
    virtual bool WriteMolecule(OBBase* pOb, OBConversion* pConv);

};

//Make an instance of the format class
FenskeZmatFormat theFenskeZmatFormat;

/////////////////////////////////////////////////////////////////

bool FenskeZmatFormat::WriteMolecule(OBBase* pOb, OBConversion* pConv)
{
    OBMol* pmol = dynamic_cast<OBMol*>(pOb);
    if (pmol == nullptr)
        return false;

    //Define some references so we can use the old parameter names
    ostream &ofs = *pConv->GetOutStream();
    OBMol &mol = *pmol;

    OBAtom *atom,*a,*b,*c;
    vector<OBAtom*>::iterator i;

    vector<OBInternalCoord*> vic;
    vic.push_back(nullptr);
    for (atom = mol.BeginAtom(i);atom;atom = mol.NextAtom(i))
        vic.push_back(new OBInternalCoord);

    CartesianToInternal(vic,mol);

    ofs << endl << mol.NumAtoms() << endl;

    double r,w,t;
    for (atom = mol.BeginAtom(i);atom;atom = mol.NextAtom(i))
    {
        a = vic[atom->GetIdx()]->_a;
        b = vic[atom->GetIdx()]->_b;
        c = vic[atom->GetIdx()]->_c;
        r = vic[atom->GetIdx()]->_dst;
        w = vic[atom->GetIdx()]->_ang;
        t = vic[atom->GetIdx()]->_tor;
        //  16 = sizeof(type)
        std::string type = OBElements::GetSymbol(atom->GetAtomicNum());

        if (atom->GetIdx() == 1)
        {
            ofs << fmt::format("{:<2s}  1\n", type);
            continue;
        }

        if (atom->GetIdx() == 2)
        {
            ofs << fmt::format("{:<2s}{:3d}{:>6.3f}\n", type, a->GetIdx(), r);
            continue;
        }

        if (atom->GetIdx() == 3)
        {
            ofs << fmt::format("{:<2s}{:3d}{:>6.3f}{:3d}{:>8.3f}\n",
                               type, a->GetIdx(), r, b->GetIdx(), w);
            continue;
        }

        if (t < 0)
            t += 360;
        ofs << fmt::format("{:<2s}{:3d}{:>6.3f}{:3d}{:>8.3f}{:3d}{:>6.1f}",
                           type, a->GetIdx(), r, b->GetIdx(), w, c->GetIdx(), t);
    }

    return(true);
}

} //namespace OpenBabel