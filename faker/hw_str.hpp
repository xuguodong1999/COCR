#ifndef _HW_STR_HPP_
#define _HW_STR_HPP_

#include "hw_item.hpp"

enum class HwCharType {
    Normal,
    RightBottom,
    RightTop,
    Up,
    Down,
    UpRand,
    DownRand,
    Cover
};

enum class HwSpecText {
    PositiveElec,
    NegativeElec
};

class HwStr : public HwItem {
    std::vector<std::pair<std::string, HwCharType>> richText;
    DetectorClasses label;
    inline static const float
            refH = 32, refX = 0, refY = 0, refCentY = 16, refSubMinH = 12,randAngle=3;

public:
    DetectorClasses getItemType() const override;

    static std::shared_ptr<HwItem> GetSpecText(const HwSpecText &_specText);

    HwStr();

    HwStr(const std::string &_plainText);

    HwStr(const ElementType &_elementType);

    /**
     * 向后添加一个带属性标注的字符
     * @param _charStr 变长字符，有的字符的长度超过了char
     * @param _hwCharType 字符属性
     */
    void push_char(const std::string &_charStr, const HwCharType &_hwCharType = HwCharType::Normal);

    std::string getShownText() const;
};

#endif//_HW_STR_HPP_
