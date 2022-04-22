#include "cocr/ocr_item.h"
#include "ocv/algorithm.h"
#include <unordered_map>
#include <utility>

struct OCRDataItem {
    rectf mRect;

    OCRDataItem(rectf _rect);

    virtual ElementType getElement() const;

    virtual const point2f &getFrom() const;

    virtual const point2f &getTo() const;

    virtual float getRadius() const;

    virtual std::string getText() const;

    virtual point2f getCenter() const;

    virtual BondType getBondType() const;
};

struct OCRLineDataItem : public OCRDataItem {

    BondType bondType;
    float length;
    point2f from, to;
    inline static int sMinSize = 5;
    inline static float sLineThresh = 2;

    OCRLineDataItem(const BondType &_bondType, const rectf &_rect)
            : bondType(_bondType), OCRDataItem(_rect) {}

    point2f getCenter() const override { return (from + to) / 2.0; }

    std::string getText() const override {
        switch (bondType) {
            case BondType::SingleBond:
                return "-";
            case BondType::DoubleBond:
                return "=";
            case BondType::TripleBond:
                return "#";
            case BondType::SolidWedgeBond:
                return "Up";
            case BondType::DashWedgeBond:
                return "Down";
            case BondType::WaveBond:
                return "~~";
            default:
                return "err";
        }
    }

    void predFromTo(const Mat &mat);

    const point2f &getFrom() const override { return from; }

    const point2f &getTo() const override { return to; }

    BondType getBondType() const override { return bondType; }
};

struct OCRCircleDataItem : public OCRDataItem {

    float radius;
    point2f center;

    std::string getText() const override { return "circle"; }

    float getRadius() const override { return radius; }

    point2f getCenter() const override { return center; }

    OCRCircleDataItem(const rectf &_rect)
            : radius(::getAvgWidth(_rect)), center(::getCenter(_rect)), OCRDataItem(_rect) {}
};

struct OCRElementDataItem : public OCRDataItem {

    ElementType element;

    ElementType getElement() const override { return element; }

    std::string getText() const override {
        return ElementUtil::convertElementTypeToString(element);
    }

    OCRElementDataItem(const ElementType &_elementType, const rectf &_rect)
            : element(_elementType), OCRDataItem(_rect) {}
};

struct OCRGroupDataItem : public OCRDataItem {

    std::string text;

    std::string getText() const override { return text; }

    OCRGroupDataItem(std::string &_text, const rectf &_rect)
            : text(std::move(_text)), OCRDataItem(_rect) {}
};

OCRDataItem::OCRDataItem(rectf _rect) : mRect(std::move(_rect)) {}

ElementType OCRDataItem::getElement() const {
    return ElementType::SA;
}

static point2f sPts0(0, 0);

const point2f &OCRDataItem::getFrom() const {
    return sPts0;
}

const point2f &OCRDataItem::getTo() const {
    return sPts0;
}

float OCRDataItem::getRadius() const {
    return 0;
}

std::string OCRDataItem::getText() const {
    return "Empty";
}

point2f OCRDataItem::getCenter() const {
    return ::getCenter(mRect);
}

BondType OCRDataItem::getBondType() const {
    return BondType::WaveBond;
}

void OCRLineDataItem::predFromTo(const Mat &_imgGray) {
    if (BondType::SolidWedgeBond == bondType || BondType::DashWedgeBond == bondType) {
        std::tie(from, to) = CvUtil::GetWedgeFromTo(_imgGray, mRect);
    } else {
        std::tie(from, to) = CvUtil::GetLineFromTo(_imgGray, mRect);
    }
}


void OCRItem::setAsLineBond(const BondType &_bt, const rectf &_rect, const Mat &_input) {
    type = OCRItemType::Line;
    auto bd = std::make_shared<OCRLineDataItem>(_bt, _rect);
    bd->predFromTo(_input);
    data = bd;
}

void OCRItem::setAsCircleBond(const rectf &_rect) {
    type = OCRItemType::Circle;
    data = std::make_shared<OCRCircleDataItem>(_rect);
}

void OCRItem::setAsText(std::string &_text, const rectf &_rect) {
    ElementType eleType = ElementUtil::convertStringToElementType(_text);
    if (eleType != ElementType::SA) {
        type = OCRItemType::Element;
        data = std::make_shared<OCRElementDataItem>(eleType, _rect);
    } else {
        type = OCRItemType::Group;
        data = std::make_shared<OCRGroupDataItem>(_text, _rect);
    }
}

ElementType OCRItem::getElement() const {
    return data->getElement();
}

const point2f &OCRItem::getFrom() const {
    return data->getFrom();
}

const point2f &OCRItem::getTo() const {
    return data->getTo();
}

float OCRItem::getRadius() const {
    return data->getRadius();
}

std::string OCRItem::getText() const {
    return data->getText();
}

const rectf &OCRItem::getRect() const {
    return data->mRect;
}

point2f OCRItem::getCenter() const {
    return data->getCenter();
}

BondType OCRItem::getBondType() const {
    return data->getBondType();
}

size_t OCRItem::getUId() const {
    return uid;
}

OCRItem::OCRItem(const size_t &_uid) : uid(_uid) {

}

void OCRItem::setUId(const size_t &_uid) {
    uid = _uid;
}
