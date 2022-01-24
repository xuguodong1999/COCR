#pragma once
#include <QObject>
#include <QVector3D>
#include <QColor>
#include <QString>

namespace Qt3DCore {
    class QTransform;

    class QEntity;
}

class BaseEntity;

class BaseWrapper : public QObject {
Q_OBJECT
protected:
    Qt3DCore::QEntity *root;
    BaseEntity *entity;
    Qt3DCore::QTransform *transform;
public:
    virtual void setObjectName(const QString &_name);

    BaseWrapper(Qt3DCore::QEntity *_root);

    ~BaseWrapper();

    void setScale(const float &_scale);

    void setTranslation(const QVector3D &_trans);

    virtual void setRindsAndSlices(const int &_rings, const int &_slices) = 0;

    virtual void setColor(const QColor &_color) = 0;

    virtual void setId(const size_t &_id);
};