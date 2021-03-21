#ifndef _MOL3D_ENTITY_HPP_
#define _MOL3D_ENTITY_HPP_

#include <QObject>
#include <QColor>
#include <QVector3D>

#include <optional>

namespace Qt3DCore {
    class QEntity;

    class QTransform;
}

namespace Qt3DExtras {
    class QPhongMaterial;

    class QSphereMesh;

    class QCylinderMesh;
}


class BaseEntity {
protected:
    Qt3DCore::QEntity *root;
    Qt3DCore::QEntity *entity;
    Qt3DCore::QTransform *transform;
public:
    BaseEntity(Qt3DCore::QEntity *_root);

    void setScale(const float &_scale);

    void setTranslation(const QVector3D &_trans);

    virtual void setRindsAndSlices(const int &_rings, const int &_slices) = 0;
};

class BaseSingleEntity : public BaseEntity {
protected:
    Qt3DExtras::QPhongMaterial *material;
public:
    BaseSingleEntity(Qt3DCore::QEntity *_root);

    void setColor(const QColor &_color);
};

class SphereEntity : public BaseSingleEntity {
    Qt3DExtras::QSphereMesh *sphere;
public:
    SphereEntity(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setColor(const QColor &_color);
};

class CylinderEntity : public BaseSingleEntity {
    friend class MultiCylinderEntity;

    Qt3DExtras::QCylinderMesh *cylinder;
public:
    CylinderEntity(Qt3DCore::QEntity *_root);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setRadius(const float &_radius);

private :
    void setLength(const float &_length);
};

class MultiCylinderEntity : public BaseEntity {
    std::vector<CylinderEntity> cylinders;
    std::vector<Qt3DExtras::QPhongMaterial *> materials;
    static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy;
public:
    MultiCylinderEntity(Qt3DCore::QEntity *_root, int _num = 1);

    void setColor(const QColor &_color);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setDistance(const float &_distance);

    void setDirection(const QVector3D &_from, const QVector3D &_to,
                      const std::optional<QVector3D> &_norm = std::nullopt);

private:
    void setTranslation(const QVector3D &_trans);
};

#endif//_MOL3D_ENTITY_HPP_
