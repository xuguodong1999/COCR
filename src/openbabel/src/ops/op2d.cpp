/**********************************************************************
op2.cpp - Plugin to adds 2D coordinates using Coordgenlibs routines

Copyright (C) 2007 by Chris Morley

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
#include <openbabel/op.h>
#include <openbabel/mol.h>
#include <openbabel/oberror.h>
#include <openbabel/obiter.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <coordgenlibs/sketcherMinimizer.h>
#include <iostream>
namespace OpenBabel
{
  class Op2D : public OBOp //was OBERROR when with OpenBabelDLL
{
public:
  Op2D(const char* ID) : OBOp(ID, false){};
  const char* Description()
  {
    return "Generate 2D coordinates\n"
      "Uses Coordgenlibs https://github.com/schrodinger/coordgenlibs";
  }
  bool WorksWith(OBBase* pOb)const{ return dynamic_cast<OBMol*>(pOb)!=NULL; }

  bool Do(OBBase* pOb, const char* OptionText, OpMap*, OBConversion* pConv=nullptr);
};

Op2D theOp2D("2D"); //Global instance

/////////////////////////////////////////////////////////////////
bool Op2D::Do(OBBase* pOb, const char* OptionText, OpMap*, OBConversion* pConv)
{
  OBMol* pmol = dynamic_cast<OBMol*>(pOb);
  if(!pmol)
    return false;

  //Do the conversion
  try {
    // sketcherMinimizer 析构的时候会 delete 所有关联的指针
    // 这里 sketcherMinimizerMolecule 一定得 new 一个，用智能指针或值就会报 double free
    sketcherMinimizer minimizer;
    auto cMol = new sketcherMinimizerMolecule();
    std::unordered_map<unsigned long, sketcherMinimizerAtom *> atomMap;
    FOR_ATOMS_OF_MOL(a, pmol) {
        auto cAtom = cMol->addNewAtom();
        cAtom->setAtomicNumber(a->GetAtomicNum());
        atomMap.insert({a->GetId(), cAtom});
    }
    FOR_BONDS_OF_MOL(b, pmol) {
        auto cBond = cMol->addNewBond(
                atomMap.at(b->GetBeginAtom()->GetId()), atomMap.at(b->GetEndAtom()->GetId()));
        cBond->setBondOrder(b->GetBondOrder());
    }
    minimizer.initialize(cMol);
    minimizer.runGenerateCoordinates();
    for (auto&[id, cAtom]: atomMap) {
        auto &p = cAtom->coordinates;
        pmol->GetAtomById(id)->SetVector(p.x(), p.y(), 0);
    }
  }
  catch (...) {
    obErrorLog.ThrowError(__FUNCTION__,
      "Op2D failed with an exception, probably in Coordgenlibs Code", obError);
    return false;
  }
  pmol->SetDimension(2); //No longer without coordinates!
  return true;
}

}//namespace
