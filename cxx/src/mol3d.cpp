#include "mol3d.hpp"
#include "qt_util.hpp"
#include "bond_type.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"

#include <Qt3DCore/QTransform>
#include <Qt3DCore/QComponent>

#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

Mol3D::Mol3D(Qt3DCore::QEntity *_rootEntity) : mRootEntity(_rootEntity) {
}

Mol3D::~Mol3D() {
}


Qt3DCore::QEntity *Mol3D::getSphereEntity(
        const QVector3D &_center, const float &_radius, const QColor &_color) {
    auto *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    sphereMesh->setRadius(_radius);

    auto *sphereTransform = new Qt3DCore::QTransform();
    sphereTransform->setScale(1.0f);
    sphereTransform->setTranslation(_center);

    auto *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(_color);

    auto sphereEntity = new Qt3DCore::QEntity(mRootEntity);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereMaterial);
    sphereEntity->addComponent(sphereTransform);
    return sphereEntity;
}

Qt3DCore::QEntity *Mol3D::getCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color) {
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(_from.distanceToPoint(_to));
    cylinder->setRings(100);
    cylinder->setSlices(20);

    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation((_from + _to) / 2.0);

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
    return cylinderEntity;
}

// 返回建议比例，以碳为基准
inline float atomRadius(const ElementType &_element) {
    switch (_element) {
        case ElementType::B:
            return 82. / 77.;
        case ElementType::C:
            return 77. / 77.;
        case ElementType::H:
            return 32. / 77.;
        case ElementType::O:
            return 73. / 77.;
        case ElementType::N:
            return 75. / 77.;
        case ElementType::P:
            return 106. / 77.;
        case ElementType::S:
            return 102. / 77.;
        case ElementType::F:
            return 72. / 77.;
        case ElementType::Cl:
            return 99. / 77.;
        case ElementType::Br:
            return 114. / 77.;
        case ElementType::I:
            return 133. / 77.;
        default:
            return 1.;
    }
}

inline QColor atomColor(const ElementType &_element) {
    switch (_element) {
        case ElementType::B:
            return getQColor(ColorName::rgbLightCyan);
        case ElementType::C:
            return getQColor(ColorName::rgbOrange);
        case ElementType::H:
            return getQColor(ColorName::rgbLightBlue);
        case ElementType::O:
            return getQColor(ColorName::rgbDarkSlateBlue);
        case ElementType::N:
            return getQColor(ColorName::rgbYellow);
        case ElementType::P:
            return getQColor(ColorName::rgbDarkGrey);
        case ElementType::S:
            return getQColor(ColorName::rgbDarkGoldenrod);
        case ElementType::F:
            return getQColor(ColorName::rgbLightSeaGreen);
        case ElementType::Cl:
            return getQColor(ColorName::rgbLightGreen);
        case ElementType::Br:
            return getQColor(ColorName::rgbDarkRed);
        case ElementType::I:
            return getQColor(ColorName::rgbPurple);
        default:
            return getQColor(ColorName::rgbBlack);
    }
}

void Mol3D::resetMol(std::shared_ptr<JMol> _mol) {
    clear();
    mol = _mol;
    if (mol->getAtomsMap().empty()) {
        std::cerr << "get empty molecule in Mol3D::resetMol" << std::endl;
        return;
    }
    const float baseSize = 200;
    // 坐标系
//    QVector3D offset(-baseSize / 3, baseSize / 3, 0);
//    getCylinderEntity(offset + zeroP, offset + axisX * 10, 0.2, Qt::blue);
//    getCylinderEntity(offset + zeroP, offset + axisY * 10, 0.2, Qt::red);
//    getCylinderEntity(offset + zeroP, offset + axisZ * 10, 0.2, Qt::green);
    auto pos3DMap = mol->get3DCoordinates(true);
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::max();
    maxx = maxy = maxz = std::numeric_limits<float>::lowest();
    for (auto&[_, p]:pos3DMap) {
        minx = (std::min)(minx, p.x);
        miny = (std::min)(miny, p.y);
        minz = (std::min)(minz, p.z);
        maxx = (std::max)(maxx, p.x);
        maxy = (std::max)(maxy, p.y);
        maxz = (std::max)(maxz, p.z);
    }
    cv::Point3f centOffset((minx + maxx) / 2, (miny + maxy) / 2, (minz + maxz) / 2);
    float k = baseSize /
              (0.001 + std::abs((std::max)((std::max)(maxx - minx, maxy - miny), maxz - minz)));
    for (auto&[_, p]:pos3DMap) {
        p -= centOffset;
        p *= k;
    }
    float avgBondLength = 0;
    if (mol->getBondsMap().size() == 0) {
        avgBondLength = baseSize;
    } else {
        auto getAvgBondLength = [&](const size_t &_bid) -> void {
            auto &bond = mol->getBondsMap().at(_bid);
            cv::Point3f from = pos3DMap[bond->getAtomFrom()];
            cv::Point3f to = pos3DMap[bond->getAtomTo()];
            avgBondLength += getDistance3D(from, to);
        };
        mol->safeTraverseBonds(getAvgBondLength);
        avgBondLength /= (std::max)((decltype(mol->getBondsMap().size())) (1),
                                    mol->getBondsMap().size());
        avgBondLength = (std::min)(avgBondLength, baseSize);
    }
    auto convert = [](const cv::Point3f &cvPts) -> QVector3D {
        return QVector3D(cvPts.x, cvPts.y, cvPts.z);
    };
    auto addAtomEntity = [&](const size_t &_aid) -> void {
        auto &ele = mol->getAtomsMap().at(_aid)->getElementType();
        cv::Point3f pos = pos3DMap[_aid];
//        qDebug() << convert(pos);
        mAtomEntities.insert({_aid, getSphereEntity(
                convert(pos), avgBondLength / 3.5 * atomRadius(ele), atomColor(ele))});
    };
    mol->safeTraverseAtoms(addAtomEntity);
    auto addBondEntity = [&](const size_t &_bid) -> void {
        auto &bond = mol->getBondsMap().at(_bid);
        cv::Point3f from = pos3DMap[bond->getAtomFrom()];
        cv::Point3f to = pos3DMap[bond->getAtomTo()];
        switch (bond->getBondType()) {
            case JBondType::SingleBond:
            case JBondType::DelocalizedBond:
            case JBondType::DashWedgeBond:
            case JBondType::SolidWedgeBond:
            case JBondType::WaveBond: {
                mBondEntities.insert({_bid, getCylinderEntity(
                        convert(from), convert(to), avgBondLength / 20,
                        getQColor(ColorName::rgbLightSkyBlue))});
                break;
            }
            case JBondType::DoubleBond: {
                auto[c1, c2]=getDoubleCylinderEntity(
                        convert(from), convert(to), avgBondLength / 30,
                        getQColor(ColorName::rgbLightSkyBlue));
                mBondEntities.insert({_bid, c1});
                mBondEntities.insert({_bid, c2});
                break;
            }
            case JBondType::TripleBond: {
                auto[c1, c2, c3]=getTripleCylinderEntity(
                        convert(from), convert(to), avgBondLength / 40,
                        getQColor(ColorName::rgbLightSkyBlue));
                mBondEntities.insert({_bid, c1});
                mBondEntities.insert({_bid, c2});
                mBondEntities.insert({_bid, c3});
                break;
            }
            default: {
                std::cerr << "unhandled bond type: "
                          << static_cast<int>(bond->getBondType()) << std::endl;
                exit(-1);
            }
        }
    };
    mol->safeTraverseBonds(addBondEntity);
}

void Mol3D::clear() {
    mol = nullptr;// free shared ptr
    for (auto&[_, entity]:mAtomEntities) {
        entity->deleteLater();
    }
    for (auto&[_, entity]:mBondEntities) {
        entity->deleteLater();
    }
    mAtomEntities.clear();
    mBondEntities.clear();
}

std::pair<Qt3DCore::QEntity *, Qt3DCore::QEntity *>
Mol3D::getDoubleCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color) {
    float length = _from.distanceToPoint(_to);
    float distance = _radius * 3;
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation(axisZ * distance + (_from + _to) / 2.0);

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity1 = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity1->addComponent(cylinder);
    cylinderEntity1->addComponent(cylinderMaterial);
    cylinderEntity1->addComponent(cylinderTransform);

    ////////////////
    cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation(axisZ * -distance + (_from + _to) / 2.0);

    cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity2 = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity2->addComponent(cylinder);
    cylinderEntity2->addComponent(cylinderMaterial);
    cylinderEntity2->addComponent(cylinderTransform);
    ////////////////////////
    return {cylinderEntity1, cylinderEntity2};
}

std::tuple<Qt3DCore::QEntity *, Qt3DCore::QEntity *, Qt3DCore::QEntity *>
Mol3D::getTripleCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color) {
    float length = _from.distanceToPoint(_to);
    float distance = _radius * 4;
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation((_from + _to) / 2.0);

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity1 = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity1->addComponent(cylinder);
    cylinderEntity1->addComponent(cylinderMaterial);
    cylinderEntity1->addComponent(cylinderTransform);

    ////////////////
    cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation(axisZ * distance + (_from + _to) / 2.0);

    cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity2 = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity2->addComponent(cylinder);
    cylinderEntity2->addComponent(cylinderMaterial);
    cylinderEntity2->addComponent(cylinderTransform);
    ////////////////////////

    ////////////////
    cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation(axisZ * -distance + (_from + _to) / 2.0);

    cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity3 = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity3->addComponent(cylinder);
    cylinderEntity3->addComponent(cylinderMaterial);
    cylinderEntity3->addComponent(cylinderTransform);
    ////////////////////////
    return {cylinderEntity1, cylinderEntity2, cylinderEntity3};
}

