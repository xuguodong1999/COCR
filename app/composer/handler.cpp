#include "handler.hpp"
#include "handler_p.hpp"
#include <QDebug>

using namespace xgd::_p;

QDebug operator<<(QDebug dbg, const cv::Point2f &p) {
    dbg << "(" << p.x << "," << p.y << ")";
    return dbg;
}

QDebug operator<<(QDebug dbg, const std::string &s) {
    dbg << s.c_str();
    return dbg;
}

std::shared_ptr<xgd::JMol> xgd::ComposerHelper::compose(const std::vector<OCRItem> &_items) {
    for (size_t i = 0; i < _items.size(); i++) {
        auto &item = _items[i];
        auto &rect = item.getRect();
        switch (item.type) {
            case OCRItemType::Element: {
                mAtoms.push_back(std::make_shared<A>(
                        mIdBase++, rect.x, rect.y, rect.width, rect.height, item.getElement()));
                break;
            }
            case OCRItemType::Group:
//                mGroups.push_back(std::make_shared<G>(
//                        mIdBase++, rect.x, rect.y, rect.width, rect.height));
                break;
            case OCRItemType::Line:
                qDebug() << "OCRItemType::Line" << item.getText() << item.getCenter();
                break;
            case OCRItemType::Circle:
                qDebug() << "OCRItemType::Circle" << item.getCenter();
                break;
            default:
                qDebug() << "error type in loop_on_items";
                break;
        }
    }
    std::unordered_map<size_t, std::shared_ptr<JAtom>> vec2atom;
    std::unordered_map<size_t, std::shared_ptr<JBond>> vec2bond;
    auto loop_on_items = [&](const OCRItem &item) {
        switch (item.type) {
            case OCRItemType::Element: {
                auto center = item.getCenter();
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
                qDebug() << "error type in loop_on_items";
                break;
        }
    };
    for (auto &item:_items) {
        loop_on_items(item);
    }
    return nullptr;
}

xgd::ComposerHelper::ComposerHelper() : mIdBase(0) {

}
