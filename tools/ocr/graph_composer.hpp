#pragma once
#include "../chem/jmol.hpp"
#include "ocr_item.hpp"
#include <utility>
#include <vector>

namespace cocr {

    class GraphComposer {
    public:
        std::shared_ptr<JMol> compose(const std::vector<OCRItem> &_items);
    };
}