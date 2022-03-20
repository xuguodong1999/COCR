#pragma once
#include <cocr_ocr_export.h>
#include "ocr/detector_object.hpp"
#include "chem/jatom.hpp"
#include "chem/jbond.hpp"
#include <string>

namespace cocr {
    enum class OCRItemType {
        Element,
        Group,
        Line,
        Circle
    };

    struct COCR_OCR_EXPORT OCRDataItem {
        cv::Rect2f rect;

        OCRDataItem(const cv::Rect2f &_rect);

        virtual ElementType getElement() const;

        virtual const cv::Point2f &getFrom() const;

        virtual const cv::Point2f &getTo() const;

        virtual float getRadius() const;

        virtual std::string getText() const;

        virtual cv::Point2f getCenter() const;

        virtual BondType getBondType() const;
    };

    class COCR_OCR_EXPORT OCRItem {
        std::shared_ptr<OCRDataItem> data;
        size_t uid;
    public:
        OCRItemType type;

        OCRItem(const size_t &_uid = 0);

        void setUId(const size_t &_uid);

        size_t getUId() const;

        void setAsLineBond(const BondType &_bt, const cv::Rect2f &_rect, const cv::Mat &_input);

        void setAsCircleBond(const cv::Rect2f &_rect);

        void setAsText(std::string &_text, const cv::Rect2f &_rect);

        ElementType getElement() const;

        const cv::Point2f &getFrom() const;

        const cv::Point2f &getTo() const;

        float getRadius() const;

        std::string getText() const;

        const cv::Rect2f &getRect() const;

        cv::Point2f getCenter() const;

        BondType getBondType() const;
    };
}