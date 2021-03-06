#ifndef _XGD_MOL3D_ADAPTER_HPP_
#define _XGD_MOL3D_ADAPTER_HPP_

#include <QObject>
#include <QHash>

#include <memory>
#include <optional>

namespace Qt3DCore {
    class QEntity;
}
namespace xgd {
    class JMol;

    class Mol3DAdapter : public QObject {
    Q_OBJECT
        std::shared_ptr<JMol> mol;
        Qt3DCore::QEntity *root;
        QHash<Qt3DCore::QEntity *, size_t> entityAtomMap, entityBondMap;

    public:
        // 约定 mol != nullptr
        explicit Mol3DAdapter(std::shared_ptr<JMol> _mol, Qt3DCore::QEntity *_root = Q_NULLPTR);

        ~Mol3DAdapter();

    private slots:

        void onBondSelected(const size_t &_bid);

    private:
        Qt3DCore::QEntity *getSphereEntity(
                const size_t &_aid, const QVector3D &_center, const float &_radius, const QColor &_color);

        Qt3DCore::QEntity *getSingleCylinderEntity(
                const QVector3D &_from, const QVector3D &_to, const float &_radius, const QColor &_color);

        Qt3DCore::QEntity *getDoubleCylinderEntity(
                const QVector3D &_from, const QVector3D &_to, const float &_radius,
                const QColor &_color, const std::optional<QVector3D> &_normVec);

        Qt3DCore::QEntity *getTripleCylinderEntity(
                const QVector3D &_from, const QVector3D &_to, const float &_radius,
                const QColor &_color, const std::optional<QVector3D> &_normVec);
    };
}
#endif//_XGD_MOL3D_ADAPTER_HPP_
