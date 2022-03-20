#pragma once
#include <cocr_ocr_export.h>

#include "chem/jmol.hpp"
#include "ocr/ocr_item.hpp"
#include <utility>
#include <vector>

namespace cocr {

    class COCR_OCR_EXPORT  GraphComposer {
    public:
        std::shared_ptr<JMol> compose(const std::vector<OCRItem> &_items);
    };
}