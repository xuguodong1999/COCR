#ifndef _XGD_GRAPH_COMPOSER_HPP_
#define _XGD_GRAPH_COMPOSER_HPP_

#include "jmol.hpp"
#include "ocr_item.hpp"
#include <utility>
#include <vector>

namespace xgd {

    class GraphComposer {
    public:
        std::shared_ptr<Molecule> compose(const std::vector<OCRItem> &_items);
    };
}
#endif//_XGD_GRAPH_COMPOSER_HPP_
