#ifndef _MOL3D_ENTITY_HPP_
#define _MOL3D_ENTITY_HPP_

#include <QObject>
#include <QColor>
#include <QVector3D>

#include <optional>
#include <memory>

namespace Qt3DCore {
    class QEntity;

    class QTransform;
}

namespace Qt3DExtras {
    class QPhongMaterial;

    class QSphereMesh;

    class QCylinderMesh;
}

namespace Qt3DInput {
    class QMouseHandler;

    class QMouseDevice;
}
namespace Qt3DRender {
    class QObjectPicker;
}
class BaseEntity : public QObject {
Q_OBJECT
protected:
    size_t id;
    Qt3DCore::QEntity *root;
    Qt3DCore::QEntity *entity;
    Qt3DCore::QTransform *transform;
    Qt3DInput::QMouseHandler *mMouseHandler;
    Qt3DInput::QMouseDevice *mMouseDevice;
    Qt3DRender::QObjectPicker *mPicker;
public:
    BaseEntity(Qt3DCore::QEntity *_root);

    void setScale(const float &_scale);

    void setTranslation(const QVector3D &_trans);

    virtual void setRindsAndSlices(const int &_rings, const int &_slices) = 0;

    virtual void setColor(const QColor &_color) = 0;

    void setId(const size_t &_id);

signals:

    void sig_bond_picked(const size_t &);

    void sig_atom_picked(const size_t &);

private slots:

    virtual void onEntityClicked() = 0;
};

class BaseSingleEntity : public BaseEntity {
Q_OBJECT
protected:
    Qt3DExtras::QPhongMaterial *material;
public:
    BaseSingleEntity(Qt3DCore::QEntity *_root);

    void setColor(const QColor &_color) override;
};

class SphereEntity : public BaseSingleEntity {
Q_OBJECT
    Qt3DExtras::QSphereMesh *sphere;
public:
    SphereEntity(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setColor(const QColor &_color) override;

private slots:

    void onEntityClicked() override;
};

class CylinderEntity : public BaseSingleEntity {
Q_OBJECT

    friend class MultiCylinderEntity;

    Qt3DExtras::QCylinderMesh *cylinder;
public:
    CylinderEntity(Qt3DCore::QEntity *_root);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setRadius(const float &_radius);

private slots:

    void onEntityClicked() override;

private:
    void setLength(const float &_length);
};

class MultiCylinderEntity : public BaseEntity {
Q_OBJECT
    std::vector<std::shared_ptr<CylinderEntity>> cylinders;
    std::vector<Qt3DExtras::QPhongMaterial *> materials;
    static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy;
public:
    MultiCylinderEntity(Qt3DCore::QEntity *_root, int _num = 1);

    void setColor(const QColor &_color) override;

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setDistance(const float &_distance);

    void setDirection(const QVector3D &_from, const QVector3D &_to,
                      const std::optional<QVector3D> &_norm = std::nullopt);

    void setRadius(const float &_radius);

private slots:

    void onEntityClicked() override;

private:
    void setTranslation(const QVector3D &_trans);
};

#endif//_MOL3D_ENTITY_HPP_
