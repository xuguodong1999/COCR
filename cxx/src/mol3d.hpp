#ifndef _MOL3D_HPP_
#define _MOL3D_HPP_

#include "mol.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QTorusMesh>

#include <unordered_map>

class Mol3D : public QObject {
Q_OBJECT
    Qt3DCore::QEntity *mRootEntity;
    std::shared_ptr<JMol> mol;
    int sphereRings, sphereSlices, cylinderRings, cylinderSlices;
    float doubleBondSpaceScale, tripleBondSpaceScale;

private:

    std::unordered_map<size_t, Qt3DCore::QEntity *> mAtomEntities;
    std::unordered_multimap<size_t, Qt3DCore::QEntity *> mBondEntities;
public:
    explicit Mol3D(Qt3DCore::QEntity *_rootEntity, std::shared_ptr<JMol> _mol = nullptr);

    ~Mol3D();

    void resetMol(std::shared_ptr<JMol> _mol);

    float getDoubleBondSpaceScale() const;

    void setDoubleBondSpaceScale(float doubleBondSpaceScale);

    float getTripleBondSpaceScale() const;

    void setTripleBondSpaceScale(float tripleBondSpaceScale);

    int getSphereRings() const;

    void setSphereRings(int sphereRings);

    int getSphereSlices() const;

    void setSphereSlices(int sphereSlices);

    int getCylinderRings() const;

    void setCylinderRings(int cylinderRings);

    int getCylinderSlices() const;

    void setCylinderSlices(int cylinderSlices);

public slots:
private:
    Qt3DCore::QEntity *getSphereEntity(const QVector3D &_center, const float &_radius,
                                       const QColor &_color);

    Qt3DCore::QEntity *getSingleCylinderEntity(const QVector3D &_from, const QVector3D &_to,
                                               const float &_radius, const QColor &_color);

    Qt3DCore::QEntity *getDoubleCylinderEntity(const QVector3D &_from, const QVector3D &_to,
                                               const float &_radius, const QColor &_color);

    Qt3DCore::QEntity *getTripleCylinderEntity(const QVector3D &_from, const QVector3D &_to,
                                               const float &_radius, const QColor &_color);

    Qt3DCore::QEntity *getMultiCylinderEntities(
            const std::vector<QVector3D> &translations, const float &_radius,
            const QColor &_color, const QVector3D &_from, const QVector3D &_to);

    void clear();
};


#endif//_MOL3D_HPP_
