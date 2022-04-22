#pragma once

#include "els_stroke_export.h"
#include "stroke/item.h"

enum class HwCharType {
    Normal,
    RightDownSmall,
    RightUp,
    RightDown
};

enum class HwSpecText {
    PositiveElec,
    NegativeElec,
    Blank,
    LineLeftBracket,
    LineRightBracket,
    TripleHorizontalLine
};

/**
 * 数据流：
 * 输入：std::vector<std::string>
 * 处理：根据 <s1,s2,...,sn> 的 si 分发类型，存在于 sTextMap 的，
 * 非空格走 GetSpecText，然后 asScript 转 HwScript，空格单独处理
 * 不存在于 sTextMap 的直接构造 HwScript，
 * 维护一个字符串的水平游标，空格直接移动游标，非空格嵌入字符并移动游标
 */
class ELS_STROKE_EXPORT HwStr : public HwItem {
    std::vector<std::pair<std::string, HwCharType>> richText;
    DetectorClasses label;
    inline static const float centHeight = 30;
    inline static const float offset = 10;
    inline static const float offsetW = 10;
    inline static const float maxHeight = centHeight + 2 * offset;
    inline static const float subCentHeight = 20;
    inline static const float subOffset = 5;
    inline static const float subOffsetW = 5;
    inline static const float subMaxHeight = subCentHeight + 2 * subOffset;
    inline static const float subStartMinRatio = 0.5;
    inline static const float randAngle = 3;
    inline static const float subCentHeight2 = 27;
    inline static const float subOffset2 = 5;
    inline static const float subOffsetW2 = 8;
    inline static const float subMaxHeight2 = subCentHeight2 + 2 * subOffset2;
    inline static const float subStartMinRatio2 = 0.3;
    inline static std::unordered_map<std::string, HwSpecText> sTextMap = {
            {"+", HwSpecText::PositiveElec},
            {"_", HwSpecText::NegativeElec},
            {" ", HwSpecText::Blank},// 空格
//            {"(",             HwSpecText::LineLeftBracket},
//            {")",             HwSpecText::LineRightBracket},
            {"#", HwSpecText::TripleHorizontalLine},
    };


    static HwCharType decideHwCharType(const std::string &_text);

    float floatX;

    void pushBlank();

    void pushHwData(HwScript &_hwScript, const HwCharType &_hwCharType);

    void pushSpecText(const HwSpecText &_specText, const HwCharType &_hwCharType);

/**
 * 向后添加一个带属性标注的字符
 * @param _charStr 变长字符，有的字符的长度超过了char
 * @param _hwCharType 字符属性
 */
    void pushCommon(const std::string &_text,
                    const HwCharType &_hwCharType = HwCharType::Normal);

public:
    std::shared_ptr<HwBase> clone() const override;

/**
 * 在当前包围盒中均匀分布所有图元的包围盒
 */
    void equalize(const float &_width);

    DetectorClasses getItemType() const override;

    static std::shared_ptr<HwItem> GetSpecText(const HwSpecText &_specText);

    HwStr();

    explicit HwStr(const ElementType &_elementType);

    std::string getShownText() const;

    void loadRichText(const std::vector<std::string> &_unlabeledText);

    void loadRichACSII(const std::string &_text);

    void loadElement(const ElementType &_elementType);

    void loadPlainText(const std::string &_plainText);
};