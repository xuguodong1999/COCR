#include "graph_composer.hpp"
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
    auto log_item = [&](const OCRItem &item) {
        switch (item.type) {
            case OCRItemType::Element:
                qDebug() << "OCRItemType::Element" << item.getText() << item.getCenter();
                break;
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
                qDebug() << "error type in log_item";
                break;
        }
    };
    for (auto &item:_items) {
        log_item(item);
    }
    return std::make_shared<xgd::JMolAdapter>();
}
