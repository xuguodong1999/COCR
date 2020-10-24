#ifndef _TEXT_HPP_
#define _TEXT_HPP_

#include "config.hpp"
#include "shape.hpp"

enum StringNote {
    Normal,
    RightButtom,
    RightTop,
    Up,
    Down,
    UpRand,
    DownRand
};
using NChar = std::pair<StringNote, string>;
using NString = vector<NChar>;

inline NString makeNotedStringByChar(const string &text, StringNote note = Normal) {
    NString ns;
    for (auto c:text) {
        ns.push_back(std::make_pair(note, string(1, c)));
    }
    return ns;
}

inline NString makeNotedString(
        const string &text, const vector<StringNote> &note) {
    NString ns;
    vector<StringNote> _note = note;
    if (_note.size() == 0) {
        _note.resize(text.size(), Normal);
    } else {
        _note.resize(text.size(), _note.back());
    }
    for (size_t i = 0; i < text.size(); i++) {
        ns.push_back(std::make_pair(_note[i], string(1, text[i])));
    }
    return ns;
}

/**
 * 作为独立分类的符号【20分类】：
 * H,Li,B,C,N,O,F,Na,Mg,Al,Si,P,S,Cl,K,Ca,Br,I
 * 带圈正电荷，带圈负电荷
 * 聚集的元素串、符号串【1分类】，需要由文本识别系统二次识别：
 * CH3COOH,COOH,...,etc.
 */
class ShapeGroup : public ShapeInterface {
public:
    vector<Shape> shapes;
    string name;
public:
    static shared_ptr<ShapeGroup> GetShapeGroup(const string &_textType = "");

    ShapeGroup() : name("ShapeGroup") { isRotateAllowed = false; }

    ShapeGroup(const NString &name) {
        isRotateAllowed = false;
        shapes.clear();
        this->name.clear();
        for (auto &c:name) {
            append(c);
            this->name.append(c.second);
        }
    }

    void mulK(float kx, float ky) override {
        for (auto &s:shapes) {
            s.mulK(kx, ky);
        }
    }

    virtual void append(const NChar &c);

    virtual void resizeTo(float w, float h, bool keepRatio = true);

    virtual void moveLeftTopTo(const cv::Point2f &leftTop);

    virtual void moveCenterTo(const cv::Point2f &newCenter);

    virtual void move(const cv::Point2f &offset);

    virtual void rotate(float angle);

    virtual void rotateBy(float angle, const cv::Point2f &cent);

    const cv::Rect2f getBoundingBox() const override;

    virtual void paintTo(cv::Mat &canvas);

    static void setSNormSizeK(float sNormSizeK);

    static void setSSubSizeK(float sSubSizeK);

    static void setSNormOffsetKx(float sNormOffsetKx);

    static void setSNormOffsetKy(float sNormOffsetKy);

    static void setSSubOffsetKx(float sSubOffsetKx);

    static void setSSubOffsetKy(float sSubOffsetKy);

    static void setSAngleK(float sAngleK);
};

#endif//_TEXT_HPP_