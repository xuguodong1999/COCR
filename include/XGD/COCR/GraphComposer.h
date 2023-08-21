#pragma once

#include "xgd_cocr_export.h"
#include "XGD/Chem/Mol.h"
#include "XGD/COCR/OCRItem.h"

#include <utility>
#include <vector>

class XGD_COCR_EXPORT GraphComposer {
public:
    std::shared_ptr<GuiMol> compose(const std::vector<OCRItem> &_items);
};