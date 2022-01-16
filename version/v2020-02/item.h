#ifndef ITEM_H
#define ITEM_H

#include "cocr.h"

#include <string>
#include <QPair>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsPathItem>

class COCR_EXPORTS Item : public QObject, public QGraphicsPathItem {
Q_OBJECT
public:
    Item(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 2
    };

    int type() const override {
        return Type;
    }

    virtual void updatePath() = 0;

private:
    bool isUpdated;
};

class BondItem;

class COCR_EXPORTS AtomItem : public Item {
public:
    AtomItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 2
    };
    QVector<const BondItem *> bitems;

    void addBond(const BondItem *b);

    void updatePath() override;

    qreal getAverageWidth() const;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;

    QPainterPath shape() const override;
};

class COCR_EXPORTS BondItem : public Item {
public:
    BondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 3
    };

    const AtomItem *from() const;

    const AtomItem *to() const;

    void setFrom(const AtomItem *a);

    void setTo(const AtomItem *a);

protected:
    std::vector<const AtomItem *> aitems;    // 根据visible确定显示长度
};

class COCR_EXPORTS SingleBondItem : public BondItem {
public:
    SingleBondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 4
    };

    void updatePath() override;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;
};

class COCR_EXPORTS DoubleBondItem : public SingleBondItem {
public:
    DoubleBondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 5
    };

    void updatePath() override;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;
};

class COCR_EXPORTS TripleBondItem : public SingleBondItem {
public:
    TripleBondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 6
    };

    void updatePath() override;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;
};

class COCR_EXPORTS SolidBondItem : public SingleBondItem {
public:
    SolidBondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 7
    };

    void updatePath() override;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;
};

class COCR_EXPORTS DashBondItem : public SingleBondItem {
public:
    DashBondItem(QGraphicsItem *parent = nullptr, const char *name = "");

    static const char *getClassName();

    enum {
        Type = UserType + 8
    };

    void updatePath() override;

    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *option,
            QWidget *widget = nullptr
    ) override;
};

#ifdef MAKE_DATASET
static qreal rAtomSide1Keep,		//化学键一端保留的比例
static qreal rAtomSide2Keep,		//化学键一端保留的比例
static qreal rBondOffsetAngle,		//化学偏移中轴的夹角
static qreal rBondAngle1,			//双键和中轴的夹角
static qreal rWedgeBondAngle,		//楔形键和中轴的夹角
static qreal rSolidBondDense,		//实楔形键的像素密度
static qreal rDashBondDense,		//虚楔形键的像素密度
static qreal rAtomTextAvgWidth,		//原子文本的大小
static qreal rAtomTextAngle,		//原子文本的旋转角度
static qreal rPenWidth;				//画笔宽度
inline void resetRandomFlag() {
    rAtomSide1Keep = 1;
    rAtomSide2Keep = 1;
    rBondOffsetAngle = 2;
    rBondAngle1 = 15;
    rBondAngle2 = 15;
    rWedgeBondAngle = 15;
    rSolidBondDense = 1;
    rDashBondDense = 1;
    rAtomTextAvgWidth = 10;
    rAtomTextAngle = 10;
    rPenWidth = 5;
}
#endif // MAKE_DATASET

#endif  // ITEM_H
