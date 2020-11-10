/**
 * FIXME: This file is shit. Dont base new codes on it.
 */
#ifndef _HW_HPP_
#define _HW_HPP_
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif //_USE_MATH_DEFINES

#include "opencv_util.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <set>
#include <string>

const cv::Scalar cvBlack = cv::Scalar(0, 0, 0);
const cv::Scalar cvWhite = cv::Scalar(255, 255, 255);
const cv::Scalar cvBlue = cv::Scalar(255, 0, 0);
const cv::Scalar cvRed = cv::Scalar(0, 0, 255);
using Point = cv::Point2f;
using Stroke = std::vector<Point>;
using Script = std::vector<Stroke>;

class ShapeInterface {
public:
    /**
     * 将当前数据画入 canvas
     * @param canvas 目标画布
     */
    virtual void paintTo(cv::Mat &canvas) = 0;

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    virtual const cv::Rect2f getBoundingBox() const = 0;

    /**
     * 顺时针旋转，绕图元中心
     * @param angle 角度制
     */
    virtual void rotate(float angle) = 0;

    /**
     * 顺时针旋转，绕点 cent
     * @param angle 角度制
     */
    virtual void rotateBy(float angle, const Point &cent) = 0;

    virtual const Point getCenter();

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    virtual void move(const Point &offset) = 0;

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    virtual void moveCenterTo(const Point &newCenter) = 0;

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    virtual void moveLeftTopTo(const Point &leftTop) = 0;

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
    virtual void resizeTo(float w, float h, bool keepRatio = true) = 0;

    /**
     * 为所有点数乘 (kx,ky)
     * @param kx
     * @param ky
     */
    virtual void mulK(float kx, float ky) = 0;

    bool IsRotateAllowed() const { return isRotateAllowed; }

protected:
    bool isRotateAllowed;

    ShapeInterface();
};

class ShapeItem : public ShapeInterface {
public:
    ShapeItem();

    virtual void paintTo(cv::Mat &canvas);

    virtual const cv::Rect2f getBoundingBox() const override;

    virtual void rotate(float angle);

    virtual void rotateBy(float angle, const cv::Point2f &cent);

    virtual void move(const cv::Point2f &offset);

    virtual void moveCenterTo(const cv::Point2f &newCenter);

    virtual void moveLeftTopTo(const cv::Point2f &leftTop);

    virtual void resizeTo(float w, float h, bool keepRatio = true);

    /**
     * 算一个映射：从【最小矩形】向【等比例的以(p1,p2)为中长轴的矩形】，为所有点做这个映射，指向随机
     * @param p1 起点
     * @param p2 终点
     */
    void castToLine(const cv::Point2f &p1, const cv::Point2f &p2, const float lThresh = 0.05);

    /**
     *
     * @param center 中心
     * @param rx 椭圆水平半径
     * @param ry 椭圆竖直半径
     */
    void castToCircle(const cv::Point2f &center, float rx, float ry);

    /**
     * 构造一个from->to的仿射变换，为所有点执行这个仿射变换
     * @param from 三角形1
     * @param to 三角形2
     */
    void castBy(const std::vector<Point> &from, const std::vector<Point> &to);

    void mulK(float kx, float ky);

    void rotateTheta(float theta);

    void rotateThetaBy(float theta, const cv::Point2f &cent);

public:
    Script mData;
    int mLabel;
public:
    Script &getData();
};

cv::Rect2f getRect(const Stroke &stroke);

cv::Rect2f getRect(const Script &script);

template<typename _Tp>
inline _Tp distance(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

class RC {
public:
    inline static float sNormSizeK = 1.0;// (-1,1)*100
    inline static float sSubSizeK = 1.0;// (-1,1)*50
    inline static float sNormOffsetKx = 0.2;// (-1,1)*100
    inline static float sNormOffsetKy = 0.2;// (-1,1)*100
    inline static float sSubOffsetKx = 0.1;// (-1,1)*50
    inline static float sSubOffsetKy = 0.1;// (-1,1)*50
    inline static float sAngleK = 0.0;// [0,1)*30
    inline static std::set<std::string> bsSet, aeSet, btSet;
    inline static std::set<int> acSet;

    struct NoiseParm {
        float mean, stddev;//gaussian
        NoiseParm() : mean(0), stddev(0.1) {}
    };

    inline static NoiseParm noiseParm;
    inline static float revertColorProb = 0.5;

    inline static float getGaussianNoiseMean() {
        return noiseParm.mean;
    }

    inline static float getGaussianNoiseStddev() {
        return belowProb(noiseParm.stddev);
    }

    struct ShapeAttr {
        //color(cvBlack), thickness(3), lineType(cv::LINE_AA), shift(0)
        int thickness;
        int lineType;
        int shift;
        cv::Scalar color;

        ShapeAttr() : color(cvBlack), thickness(2), lineType(cv::LINE_AA), shift(0) {}
    };

    static void update_shape_attr() {
    }

    inline static ShapeAttr shapeAttr;

    static const int get_shape_shift() {
        return shapeAttr.shift;
    }

    inline static std::vector<int> thicknessVec = {2};

    static const int get_shape_thickness() {
        return thicknessVec[rand() % thicknessVec.size()];
    }

    static const int get_shape_lineType() {
        return shapeAttr.lineType;
    }

    static const cv::Scalar get_shape_color() {
        return shapeAttr.color;
    }
};

enum StringNote {
    Normal,
    RightButtom,
    RightTop,
    Up,
    Down,
    UpRand,
    DownRand
};
using NChar = std::pair<StringNote, std::string>;
using NString = std::vector<NChar>;

inline NString makeNotedStringByChar(const std::string &text, StringNote note = Normal) {
    NString ns;
    for (auto c:text) {
        ns.push_back(std::make_pair(note, std::string(1, c)));
    }
    return ns;
}

inline NString makeNotedString(
        const std::string &text, const std::vector<StringNote> &note) {
    NString ns;
    std::vector<StringNote> _note = note;
    if (_note.size() == 0) {
        _note.resize(text.size(), Normal);
    } else {
        _note.resize(text.size(), _note.back());
    }
    for (size_t i = 0; i < text.size(); i++) {
        ns.push_back(std::make_pair(_note[i], std::string(1, text[i])));
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
    std::vector<ShapeItem> shapes;
    std::string name;
public:
    static std::shared_ptr<ShapeGroup> GetShapeGroup(const std::string &_textType = "");

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

/**
 * 单个化学键【7分类】：
 * [-1,-7]
 * 圆圈、单键、双键、三键、实楔形键、虚楔形键、波浪线
 */
class BondItem : public ShapeGroup {
protected:
    BondItem(const std::string &_bondType);

    bool mUseHWChar;
    bool isLatest;

    virtual void updateShapes();

public:
    void setUseHandWrittenWChar(bool useHandWrittenChar);

    static std::shared_ptr<BondItem> GetBond(const std::string &_bondType = "Single");

    virtual void setVertices(const std::vector<Point> &pts) = 0;
};

class CircleBond : public BondItem {
    Point center;
    float r;

    void updateShapes() override;

public:
    CircleBond(const std::string &_bondType);

    const cv::Rect2f getBoundingBox() const override;

    void mulK(float kx, float ky);

    void resizeTo(float w, float h, bool keepRatio = true) override;

    void moveLeftTopTo(const Point &leftTop) override;

    void moveCenterTo(const Point &newCenter) override;

    void move(const Point &offset) override;

    void rotate(float angle) override;

    void rotateBy(float angle, const Point &cent) override;

    void paintTo(cv::Mat &canvas) override;

    void setVertices(const std::vector<Point> &pts) override;
};

class SingleBond : public BondItem {
    virtual void updateShapes() override;

protected:
    Point from, to;
public:
    const cv::Rect2f getBoundingBox() const override;

    SingleBond(const std::string &_bondType);

    void mulK(float kx, float ky);

    void resizeTo(float w, float h, bool keepRatio = true) override;

    void moveLeftTopTo(const Point &leftTop) override;

    void moveCenterTo(const Point &newCenter) override;

    void move(const Point &offset) override;

    void rotate(float angle) override;

    void rotateBy(float angle, const Point &cent) override;

    virtual void paintTo(cv::Mat &canvas) override;

    void setVertices(const std::vector<Point> &pts) override;
};

class DoubleBond : public SingleBond {
    void updateShapes() override;

public:
    DoubleBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class TripleBond : public SingleBond {
    void updateShapes() override;

public:
    TripleBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class SolidWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    SolidWedgeBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class DashWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    DashWedgeBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class WaveBond : public SingleBond {
    void updateShapes() override;

public:
    WaveBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

#endif//_HW_HPP_
