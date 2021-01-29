#ifndef _SOSO17_CONVERTER_HPP_
#define _SOSO17_CONVERTER_HPP_

#include "mol_holder.hpp"
#include "yolo_object.hpp"
#include "bond_type.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/core/mat.hpp"

#include <utility>

extern std::vector<std::string> CLASSES;

class SOSO17Converter : public MolHolder {

    enum class ItemType {
        BondFrom, BondTo, BondIn,
        ExplicitAtom, ImplicitAtom
    };

    // 【关系对聚类】过程 使用下面两个超参
    // 特征值小于该数的【图元对】成为【候选】关系对
    inline static float sQuotaThresh = (1.8 + 0.8) / 2;
    // 【键端-键端】特征值的放大倍数，认为【字符-键端】比【键端-键端】有更大的冗余度
    inline static float sBondExpandThresh = 3;

    struct ImplicitAtomItem {
        ElementType elementType;
        cv::Point2f center;

        ImplicitAtomItem(const float &_x, const float &_y,
                         const ElementType &_elementType = ElementType::C)
                : center(_x, _y), elementType(_elementType) {}
    };

    struct ExplicitAtomItem : public ImplicitAtomItem {
        float w, h;

        ExplicitAtomItem(const float &_x, const float &_y, const float &_w, const float &_h,
                         const ElementType &_elementType) : w(_w), h(_h), ImplicitAtomItem(
                _x + _w / 2, _y + _h / 2, _elementType) {}
    };

    struct LineBondItem {
        cv::Rect2f rect;
        JBondType bondType;
        cv::Point2f from, to;
        float length;

        LineBondItem(const float &_x, const float &_y, const float &_w, const float &_h,
                     const JBondType &_bondType)
                : rect(_x, _y, _w, _h), bondType(_bondType), length(0.1) {}

        void predFromTo(const cv::Mat &_img);

    private:
        void predFromToForLine(const cv::Mat &_imgGray);

        void predFromToForWedge(const cv::Mat &_imgGray);

        inline static int sMinSize = 5;
        inline static float sLineThresh = 2;
    };

    struct CircleBondItem {
        cv::Point2f center;
        float radius;

        CircleBondItem(const float &_x, const float &_y, const float &_r)
                : center(_x, _y), radius(_r) {}
    };

    std::vector<ExplicitAtomItem> chars;
    std::vector<LineBondItem> lines;
    std::vector<CircleBondItem> circles;

    cv::Mat imgGray;

    void clear();

public:
    SOSO17Converter();

    void accept(const cv::Mat &_img, const std::vector<YoloObject> &_objs);

    void then();
};


ElementType convertLabelToElementType(const int &_label);

JBondType convertLabelToBondType(const int &_label);

bool isBondOrStr(const int &_label);

#endif //_SOSO17_CONVERTER_HPP_
