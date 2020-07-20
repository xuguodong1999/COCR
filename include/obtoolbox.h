#ifndef _OBTOOLBOX_H_
#define _OBTOOLBOX_H_

#include "cocr.h"
#include <QApplication>
#include <string>
#include <iostream>
#include <memory>

namespace cocr {
    class OBToolBox {
    public:
        static OBToolBox &getInstance() {
            static OBToolBox ob;
            return ob;
        }

        enum class OBForceField {
            MMFF94, UFF, GAFF, GHEMICAL
        };
        enum class OBMolFormat {
            SMILES,
            MOL2,
            PDB,
            CIF
        };

        ErrorCode smiles2coor3d(const std::string &smiles, std::string &result,
                                const OBMolFormat &outputFormat = OBMolFormat::MOL2,
                                const OBForceField &forceFieldType = OBForceField::UFF);


    private:
        OBToolBox() ;

        ~OBToolBox() {}

        OBToolBox(const OBToolBox &) = delete;

        const OBToolBox &operator=(const OBToolBox &) = delete;
    };
}
#endif //_OBTOOLBOX_H_
