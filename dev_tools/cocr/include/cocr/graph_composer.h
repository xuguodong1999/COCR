#pragma once

#include "els_cocr_export.h"

#include "ckit/mol.h"
#include "cocr/ocr_item.h"
#include <utility>
#include <vector>

class ELS_COCR_EXPORT GraphComposer {
public:
    std::shared_ptr<GuiMol> compose(const std::vector<OCRItem> &_items);
};