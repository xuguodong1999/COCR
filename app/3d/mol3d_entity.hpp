#ifndef _MOL3D_ENTITY_HPP_
#define _MOL3D_ENTITY_HPP_

#include <Qt3DCore/QEntity>
#include <QObject>
#include <QColor>
#include <QVector3D>

#include <optional>
#include <memory>

namespace Qt3DCore {
    class QTransform;
}

namespace Qt3DExtras {
    class QPhongMaterial;

    class QSphereMesh;

    class QCylinderMesh;

    class QConeMesh;
}

namespace Qt3DRender {
    class QObjectPicker;

    class QPickEvent;
}

enum class EntityType {
    AtomSphere, BondCylinder, ConeMesh
};

class BaseEntity : public Qt3DCore::QEntity {
Q_OBJECT

    Qt3DRender::QObjectPicker *mPicker;
    size_t mId;
    EntityType type;
public:
    static const size_t sAxisId = std::numeric_limits<size_t>::max();

    // 必须指定父节点，不手动管理 BaseEntity类的生命周期
    explicit BaseEntity(QNode *parent);

    void setType(const EntityType &_type);

    void setId(const size_t &_id);

public slots:

    void onPicked(Qt3DRender::QPickEvent *event);

signals:

    void sig_bond_picked(const size_t &);

    void sig_atom_picked(const size_t &);
};

class BaseWrapper : public QObject {
Q_OBJECT
protected:
    Qt3DCore::QEntity *root;
    BaseEntity *entity;
    Qt3DCore::QTransform *transform;
public:
    virtual void setObjectName(const QString &_name);

    BaseWrapper(Qt3DCore::QEntity *_root);

    void setScale(const float &_scale);

    void setTranslation(const QVector3D &_trans);

    virtual void setRindsAndSlices(const int &_rings, const int &_slices) = 0;

    virtual void setColor(const QColor &_color) = 0;

    virtual void setId(const size_t &_id);
};

class SingleWrapper : public BaseWrapper {
Q_OBJECT
protected:
    Qt3DExtras::QPhongMaterial *material;
public:
    SingleWrapper(Qt3DCore::QEntity *_root);

    void setColor(const QColor &_color) override;
};

class SphereWrapper : public SingleWrapper {
Q_OBJECT
    Qt3DExtras::QSphereMesh *sphere;
public:
    SphereWrapper(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;
};

class ConeWrapper : public SingleWrapper {
Q_OBJECT
    Qt3DExtras::QConeMesh *cone;
public:
    ConeWrapper(Qt3DCore::QEntity *_root);

    void setRadius(const float &_radius);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

private:
    void setHeight(const float &_height);
};

class CylinderWrapper : public SingleWrapper {
Q_OBJECT

    friend class MultiCylinderWrapper;

    Qt3DExtras::QCylinderMesh *cylinder;
public:
    CylinderWrapper(Qt3DCore::QEntity *_root);

    void setDirection(const QVector3D &_from, const QVector3D &_to);

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setRadius(const float &_radius);

private:
    void setLength(const float &_length);
};

class MultiCylinderWrapper : public BaseWrapper {
Q_OBJECT
    std::vector<std::shared_ptr<CylinderWrapper>> cylinders;
    std::vector<Qt3DExtras::QPhongMaterial *> materials;
    static std::unordered_map<decltype(cylinders.size()), std::vector<QVector3D>> sTransStrategy;
public:
    MultiCylinderWrapper(Qt3DCore::QEntity *_root, int _num = 1);

    void setColor(const QColor &_color) override;

    void setRindsAndSlices(const int &_rings, const int &_slices) override;

    void setDistance(const float &_distance);

    void setDirection(const QVector3D &_from, const QVector3D &_to,
                      const std::optional<QVector3D> &_norm = std::nullopt);

    void setRadius(const float &_radius);

    void setObjectName(const QString &_name) override;

    void setId(const size_t &_id) override;

private:
    void setTranslation(const QVector3D &_trans);

    void loopMultiWrappers(std::function<void(CylinderWrapper &)> _func);
};

#endif//_MOL3D_ENTITY_HPP_
