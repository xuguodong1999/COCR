#include "ocr_types.hpp"
#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>
#include <unordered_map>

namespace xgd {
    std::vector<std::string> ELEMENT_NAME_LIST = {
            "None",
            "H",
            "He",
            "Li",
            "Be",
            "B",
            "C",
            "N",
            "O",
            "F",
            "Ne",
            "Na",
            "Mg",
            "Al",
            "Si",
            "P",
            "S",
            "Cl",
            "Ar",
            "K",
            "Ca",
            "Sc",
            "Ti",
            "V",
            "Cr",
            "Mn",
            "Fe",
            "Co",
            "Ni",
            "Cu",
            "Zn",
            "Ga",
            "Ge",
            "As",
            "Se",
            "Br",
            "Kr",
            "Rb",
            "Sr",
            "Y",
            "Zr",
            "Nb",
            "Mo",
            "Tc",
            "Ru",
            "Rh",
            "Pd",
            "Ag",
            "Cd",
            "In",
            "Sn",
            "Sb",
            "Te",
            "I",
            "Xe",
            "Cs",
            "Ba",
            "La",
            "Ce",
            "Pr",
            "Nd",
            "Pm",
            "Sm",
            "Eu",
            "Gd",
            "Tb",
            "Dy",
            "Ho",
            "Er",
            "Tm",
            "Yb",
            "Lu",
            "Hf",
            "Ta",
            "W",
            "Re",
            "Os",
            "Ir",
            "Pt",
            "Au",
            "Hg",
            "Tl",
            "Pb",
            "Bi",
            "Po",
            "At",
            "Rn",
            "Fr",
            "Ra",
            "Ac",
            "Th",
            "Pa",
            "U",
            "Np",
            "Pu",
            "Am",
            "Cm",
            "Bk",
            "Cf",
            "Es",
            "Fm",
            "Md",
            "No",
            "Lr",
            "Rf",
            "Db",
            "Sg",
            "Bh",
            "Hs",
            "Mt",
            "Ds",
            "Rg",
            "Cn"
    };

    static std::unordered_map<std::string, ElementType> STR_ELEMENT_SET = {
            {"None", ElementType::None},
            {"H",    ElementType::H},
            {"He",   ElementType::He},
            {"Li",   ElementType::Li},
            {"Be",   ElementType::Be},
            {"B",    ElementType::B},
            {"C",    ElementType::C},
            {"N",    ElementType::N},
            {"O",    ElementType::O},
            {"F",    ElementType::F},
            {"Ne",   ElementType::Ne},
            {"Na",   ElementType::Na},
            {"Mg",   ElementType::Mg},
            {"Al",   ElementType::Al},
            {"Si",   ElementType::Si},
            {"P",    ElementType::P},
            {"S",    ElementType::S},
            {"Cl",   ElementType::Cl},
            {"Ar",   ElementType::Ar},
            {"K",    ElementType::K},
            {"Ca",   ElementType::Ca},
            {"Sc",   ElementType::Sc},
            {"Ti",   ElementType::Ti},
            {"V",    ElementType::V},
            {"Cr",   ElementType::Cr},
            {"Mn",   ElementType::Mn},
            {"Fe",   ElementType::Fe},
            {"Co",   ElementType::Co},
            {"Ni",   ElementType::Ni},
            {"Cu",   ElementType::Cu},
            {"Zn",   ElementType::Zn},
            {"Ga",   ElementType::Ga},
            {"Ge",   ElementType::Ge},
            {"As",   ElementType::As},
            {"Se",   ElementType::Se},
            {"Br",   ElementType::Br},
            {"Kr",   ElementType::Kr},
            {"Rb",   ElementType::Rb},
            {"Sr",   ElementType::Sr},
            {"Y",    ElementType::Y},
            {"Zr",   ElementType::Zr},
            {"Nb",   ElementType::Nb},
            {"Mo",   ElementType::Mo},
            {"Tc",   ElementType::Tc},
            {"Ru",   ElementType::Ru},
            {"Rh",   ElementType::Rh},
            {"Pd",   ElementType::Pd},
            {"Ag",   ElementType::Ag},
            {"Cd",   ElementType::Cd},
            {"In",   ElementType::In},
            {"Sn",   ElementType::Sn},
            {"Sb",   ElementType::Sb},
            {"Te",   ElementType::Te},
            {"I",    ElementType::I},
            {"Xe",   ElementType::Xe},
            {"Cs",   ElementType::Cs},
            {"Ba",   ElementType::Ba},
            {"La",   ElementType::La},
            {"Ce",   ElementType::Ce},
            {"Pr",   ElementType::Pr},
            {"Nd",   ElementType::Nd},
            {"Pm",   ElementType::Pm},
            {"Sm",   ElementType::Sm},
            {"Eu",   ElementType::Eu},
            {"Gd",   ElementType::Gd},
            {"Tb",   ElementType::Tb},
            {"Dy",   ElementType::Dy},
            {"Ho",   ElementType::Ho},
            {"Er",   ElementType::Er},
            {"Tm",   ElementType::Tm},
            {"Yb",   ElementType::Yb},
            {"Lu",   ElementType::Lu},
            {"Hf",   ElementType::Hf},
            {"Ta",   ElementType::Ta},
            {"W",    ElementType::W},
            {"Re",   ElementType::Re},
            {"Os",   ElementType::Os},
            {"Ir",   ElementType::Ir},
            {"Pt",   ElementType::Pt},
            {"Au",   ElementType::Au},
            {"Hg",   ElementType::Hg},
            {"Tl",   ElementType::Tl},
            {"Pb",   ElementType::Pb},
            {"Bi",   ElementType::Bi},
            {"Po",   ElementType::Po},
            {"At",   ElementType::At},
            {"Rn",   ElementType::Rn},
            {"Fr",   ElementType::Fr},
            {"Ra",   ElementType::Ra},
            {"Ac",   ElementType::Ac},
            {"Th",   ElementType::Th},
            {"Pa",   ElementType::Pa},
            {"U",    ElementType::U},
            {"Np",   ElementType::Np},
            {"Pu",   ElementType::Pu},
            {"Am",   ElementType::Am},
            {"Cm",   ElementType::Cm},
            {"Bk",   ElementType::Bk},
            {"Cf",   ElementType::Cf},
            {"Es",   ElementType::Es},
            {"Fm",   ElementType::Fm},
            {"Md",   ElementType::Md},
            {"No",   ElementType::No},
            {"Lr",   ElementType::Lr},
            {"Rf",   ElementType::Rf},
            {"Db",   ElementType::Db},
            {"Sg",   ElementType::Sg},
            {"Bh",   ElementType::Bh},
            {"Hs",   ElementType::Hs},
            {"Mt",   ElementType::Mt},
            {"Ds",   ElementType::Ds},
            {"Rg",   ElementType::Rg},
            {"Cn",   ElementType::Cn}
    };

    struct OCRLineDataItem : public OCRDataItem {
        BondType bondType;
        float length;
        cv::Point2f from, to;
        inline static int sMinSize = 5;
        inline static float sLineThresh = 2;

        OCRLineDataItem(const BondType &_bondType, const cv::Rect2f &_rect)
                : bondType(_bondType), OCRDataItem(_rect) {}

        cv::Point2f getCenter() const override { return (from + to) / 2.0; }

        void predFromTo(const cv::Mat &_imgGray);

        void predFromToForLine(const cv::Mat &_imgGray);

        void predFromToForWedge(const cv::Mat &_imgGray);

        cv::Point2f getFrom() const override { return from; }

        cv::Point2f getTo() const override { return to; }
    };

    struct OCRCircleDataItem : public OCRDataItem {
        float radius;
        cv::Point2f center;

        float getRadius() const override { return radius; }

        cv::Point2f getCenter() const override { return center; }

        OCRCircleDataItem(const cv::Rect2f &_rect)
                : radius((_rect.width + _rect.height) / 2.0), center(getRectCenter2D(_rect)), OCRDataItem(_rect) {}
    };

    struct OCRElementDataItem : public OCRDataItem {
        ElementType element;

        ElementType getElement() const override { return element; }

        std::string getText() const override { return ELEMENT_NAME_LIST[static_cast<size_t>(element)]; }

        OCRElementDataItem(const ElementType &_elementType, const cv::Rect2f &_rect)
                : element(_elementType), OCRDataItem(_rect) {}
    };

    struct OCRGroupDataItem : public OCRDataItem {
        std::string text;

        std::string getText() const override { return text; }

        OCRGroupDataItem(std::string &_text, const cv::Rect2f &_rect)
                : text(std::move(_text)), OCRDataItem(_rect) {}
    };

    OCRDataItem::OCRDataItem(const cv::Rect2f &_rect) : rect(_rect) {}

    ElementType OCRDataItem::getElement() const {
        return ElementType::None;
    }

    cv::Point2f OCRDataItem::getFrom() const {
        return cv::Point2f(0, 0);
    }

    cv::Point2f OCRDataItem::getTo() const {
        return cv::Point2f(0, 0);
    }

    float OCRDataItem::getRadius() const {
        return 0;
    }

    std::string OCRDataItem::getText() const {
        return "Empty";
    }

    cv::Point2f OCRDataItem::getCenter() const {
        return cv::Point2f(0, 0);
    }
}

void xgd::OCRLineDataItem::predFromTo(const cv::Mat &_imgGray) {
    if (BondType::UpBond == bondType || BondType::DownBond == bondType) {
        predFromToForWedge(_imgGray);
    } else {
        predFromToForLine(_imgGray);
    }
}

void xgd::OCRLineDataItem::predFromToForLine(const cv::Mat &_imgGray) {
    float w = rect.width, h = rect.height, x = rect.x, y = rect.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(_imgGray.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(_imgGray.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
        }
        return;
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(_imgGray(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", _imgGray(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    if (get_bw_tl_br() < get_bw_bl_tr()) {
        from = rect.tl();
        to = rect.br();
    } else {
        from.x = rect.x;
        to.y = rect.y;
        from.y = rect.y + rect.height;
        to.x = rect.x + rect.width;
    }
}

void xgd::OCRLineDataItem::predFromToForWedge(const cv::Mat &_imgGray) {
    float w = rect.width, h = rect.height, x = rect.x, y = rect.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(_imgGray.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(_imgGray.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
            auto s1 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin, iw, ih / 2))));
            auto s2 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin + ih / 2, iw, ih / 2))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
            auto s1 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin, iw / 2, ih))));
            auto s2 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin + ih / 2, ymin, iw / 2, ih))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        }
        return;
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(_imgGray(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2))));
    };
    auto get_bw_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
    auto get_bw_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", _imgGray(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    auto s1 = get_bw_tl(), s2 = get_bw_tr(), s3 = get_bw_bl(), s4 = get_bw_br();
    if (s1 + s4 < s2 + s3) {
        from = rect.tl();
        to = rect.br();
        if (s1 < s4) {
            std::swap(from, to);
        }
    } else {
        from.x = rect.x;
        to.y = rect.y;
        from.y = rect.y + rect.height;
        to.x = rect.x + rect.width;
        if (s3 < s2) {
            std::swap(from, to);
        }
    }

//    std::cout<<s1<<","<<s2<<"\n"<<s3<<","<<s4<<"\n-------"<<std::endl;
//    cv::imshow("roi64", roiArea);
//    cv::Mat displayImg;
//    cv::cvtColor(_imgGray,displayImg,cv::COLOR_GRAY2BGR);
//    cv::rectangle(displayImg, rect, getScalar(ColorName::rgbGreen), 2);
//    cv::circle(displayImg, from, 4, getScalar(ColorName::rgbBlue), -1);
//    cv::circle(displayImg, to, 4, getScalar(ColorName::rgbRed), -1);
//    cv::imshow("bond", displayImg);
//    cv::waitKey(0);
}

void xgd::OCRItem::setAsLineBond(const xgd::BondType &_bt, const cv::Rect2f &_rect, const cv::Mat &_input) {
    type = OCRItemType::Line;
    auto bd = std::make_shared<OCRLineDataItem>(_bt, _rect);
    bd->predFromTo(_input);
    data = bd;
}

void xgd::OCRItem::setAsCircleBond(const cv::Rect2f &_rect) {
    type = OCRItemType::Circle;
    data = std::make_shared<OCRCircleDataItem>(_rect);
}

void xgd::OCRItem::setAsText(std::string &_text, const cv::Rect2f &_rect) {
    auto it = STR_ELEMENT_SET.find(_text);
    if (STR_ELEMENT_SET.end() != it) {
        type = OCRItemType::Element;
        data = std::make_shared<OCRElementDataItem>(it->second, _rect);
    } else {
        type = OCRItemType::Group;
        data = std::make_shared<OCRGroupDataItem>(_text, _rect);
    }
}

xgd::ElementType xgd::OCRItem::getElement() const {
    return data->getElement();
}

cv::Point2f xgd::OCRItem::getFrom() const {
    return data->getFrom();
}

cv::Point2f xgd::OCRItem::getTo() const {
    return data->getTo();
}

float xgd::OCRItem::getRadius() const {
    return data->getRadius();
}

std::string xgd::OCRItem::getText() const {
    return data->getText();
}

const cv::Rect2f &xgd::OCRItem::getRect() const {
    return data->rect;
}

cv::Point2f xgd::OCRItem::getCenter() const {
    return data->getCenter();
}
