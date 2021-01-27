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

    struct ExplicitAtomItem {
        ElementType elementType;
        cv::Point2f center;
        float w, h;

        ExplicitAtomItem(const float &_x, const float &_y, const float &_w, const float &_h,
                         const ElementType &_elementType)
                : center(_x + _w / 2, _y + _h / 2), w(_w), h(_h),
                  elementType(_elementType) {}
    };

    struct LineBondItem {
        cv::Rect2f rect;
        JBondType bondType;
        cv::Point2f from, to;

        LineBondItem(const float &_x, const float &_y, const float &_w, const float &_h,
                     const JBondType &_bondType)
                : rect(_x, _y, _w, _h), bondType(_bondType) {}
        void predFromTo(const cv::Mat&_img);

    private:
        void predFromToForLine(const cv::Mat&_imgGray);
        void predFromToForWedge(const cv::Mat&_imgGray);
        inline static int sMinSize=5;
        inline static float sLineThresh=2;
    };

    struct CircleBondItem {
        cv::Point2f center;
        float radius;

        CircleBondItem(const float &_x, const float &_y, const float &_r)
                : center(_x, _y), radius(_r) {}
    };

    std::vector<ExplicitAtomItem> explicitAtoms;
    std::vector<LineBondItem> lines;
    std::vector<CircleBondItem> circles;
    cv::Mat imgGray;

    void clear();

public:
    SOSO17Converter();

    void accept(const cv::Mat &_img, const std::vector<YoloObject> &_objs);
};

std::pair<cv::Point2f, cv::Point2f> extractBondFromTo(
        const cv::Mat &_img, const YoloObject &_obj);

ElementType convertLabelToElementType(const int &_label);

JBondType convertLabelToBondType(const int &_label);

bool isBondOrStr(const int &_label);

#endif //_SOSO17_CONVERTER_HPP_
