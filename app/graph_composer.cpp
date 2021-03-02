#include "graph_composer.hpp"
#include "jmol_adapter.hpp"

std::shared_ptr<xgd::JMol> xgd::GraphComposer::compose(const std::vector<OCRItem> &_items) {
    return std::make_shared<xgd::JMolAdapter>();
}
