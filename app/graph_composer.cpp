#include "graph_composer.hpp"
#include "composer/data_def.hpp"
#include "composer/handler.hpp"
#include "jmol_adapter.hpp"
#include <QDebug>

QDebug operator<<(QDebug dbg, const cv::Point2f &p) {
    dbg << "(" << p.x << "," << p.y << ")";
    return dbg;
}

QDebug operator<<(QDebug dbg, const std::string &s) {
    dbg << s.c_str();
    return dbg;
}

std::shared_ptr<xgd::JMol> xgd::GraphComposer::compose(const std::vector<OCRItem> &_items) {
    auto mol = std::make_shared<xgd::JMolAdapter>();
    std::unordered_map<size_t, std::shared_ptr<JAtom>> vec2atom;
    std::unordered_map<size_t, std::shared_ptr<JBond>> vec2bond;
    auto handle_item_step1 = [&](const OCRItem &item) {
        switch (item.type) {
            case OCRItemType::Element: {
                auto center = item.getCenter();
                vec2atom[item.getUId()] = mol->addAtom(item.getElement(), center.x, center.y);
                qDebug() << "OCRItemType::Element" << item.getText() << center;
                break;
            }
            case OCRItemType::Group:
                qDebug() << "OCRItemType::Group" << item.getText() << item.getCenter();
                break;
            case OCRItemType::Line:
                qDebug() << "OCRItemType::Line" << item.getText() << item.getCenter();
                break;
            case OCRItemType::Circle:
                qDebug() << "OCRItemType::Circle" << item.getCenter();
                break;
            default:
                qDebug() << "error type in handle_item_step1";
                break;
        }
    };
    for (auto &item:_items) {
        handle_item_step1(item);
    }
    return mol;
}
