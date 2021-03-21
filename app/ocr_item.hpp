#ifndef _XGD_OCR_TYPES_HPP_
#define _XGD_OCR_TYPES_HPP_

#include "detector_object.hpp"
#include "jatom.hpp"
#include "jbond.hpp"
#include <string>

namespace xgd {
    enum class OCRItemType {
        Element,
        Group,
        Line,
        Circle
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

        virtual BondType getBondType() const;
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

        BondType getBondType() const;
    };
}
#endif//_XGD_OCR_TYPES_HPP_