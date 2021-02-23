#ifndef _XGD_OCR_TYPES_HPP_
#define _XGD_OCR_TYPES_HPP_

#include <opencv2/core/types.hpp>

namespace xgd {
    enum class DetectorObjectType {
        SingleLine = 0,
        DoubleLine = 1,
        TripleLine = 2,
        SolidWedge = 3,
        DashWedge = 4,
        WaveLine = 5,
        Circle = 6,
        Text = 7
    };

    struct DetectorObject {
        inline static int maxLabel = 7, minLabel = 0;

        static bool isValidLabel(const int &_label);

        const cv::Rect2f rect;
        float prob;
        DetectorObjectType label;

        DetectorObject(const float &_x, const float &_y, const float &_w, const float &_h,
                       const int &_label, const float &_prob = 1);

        const cv::Rect2f &asRect() const;

        const float &x() const;

        const float &y() const;

        const float &w() const;

        const float &h() const;
    };

    enum class OCRItemType {
        Element,
        Group,
        Line,
        Circle
    };
    enum class ElementType : size_t {
        None = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
        Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
        Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn,
        Ga, Ge, As, Se, Br, Kr,
        Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd,
        In, Sn, Sb, Te, I, Xe,
        Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy,
        Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt,
        Au, Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th,
        Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No,
        Lr, Rf, Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
    };
    enum class BondType : int {
        ImplicitBond = 0,
        SingleBond = 1,
        DoubleBond = 2,
        TripleBond = 3,
        UpBond = 4,
        DownBond = 5,
        DelocalizedBond = 6
    };

    struct OCRDataItem {
        cv::Rect2f rect;

        OCRDataItem(const cv::Rect2f &_rect);

        virtual ElementType getElement() const;

        virtual cv::Point2f getFrom() const;

        virtual cv::Point2f getTo() const;

        virtual float getRadius() const;

        virtual std::string getText() const;

        virtual cv::Point2f getCenter() const;

        virtual BondType getBondType()const;
    };

    struct OCRItem {
        OCRItemType type;
        std::shared_ptr<OCRDataItem> data;

        void setAsLineBond(const BondType &_bt, const cv::Rect2f &_rect, const cv::Mat &_input);

        void setAsCircleBond(const cv::Rect2f &_rect);

        void setAsText(std::string &_text, const cv::Rect2f &_rect);

        ElementType getElement() const;

        cv::Point2f getFrom() const;

        cv::Point2f getTo() const;

        float getRadius() const;

        std::string getText() const;

        const cv::Rect2f &getRect() const;

        cv::Point2f getCenter() const;
        BondType getBondType()const;
    };

    inline BondType toBondType(const DetectorObjectType &_objType) {
        switch (_objType) {
            case DetectorObjectType::SingleLine :
                return BondType::SingleBond;
            case DetectorObjectType::DoubleLine  :
                return BondType::DoubleBond;
            case DetectorObjectType::TripleLine  :
                return BondType::TripleBond;
            case DetectorObjectType::SolidWedge  :
                return BondType::UpBond;
            case DetectorObjectType::DashWedge  :
                return BondType::DownBond;
            case DetectorObjectType::WaveLine  :
                return BondType::ImplicitBond;
            default: {
                throw std::runtime_error("toBondType: unknown bondType");
            }
        }
    }
}
#endif//_XGD_OCR_TYPES_HPP_
