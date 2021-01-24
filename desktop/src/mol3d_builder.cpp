#include "mol3d_builder.hpp"
#include "qt_util.hpp"
#include "std_util.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"

#include <Qt3DCore/QTransform>
//#include <Qt3DCore/QComponent>

//#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
//#include <Qt3DExtras/QCuboidMesh>
//#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

std::unordered_map<ElementType, ColorName> colorMap = {
        {ElementType::H,  ColorName::rgbLightBlue},
        {ElementType::B,  ColorName::rgbLightCyan},
        {ElementType::C,  ColorName::rgbOrange},
        {ElementType::N,  ColorName::rgbYellow},
        {ElementType::O,  ColorName::rgbDarkSlateBlue},
        {ElementType::F,  ColorName::rgbLightSeaGreen},
        {ElementType::P,  ColorName::rgbDarkGrey},
        {ElementType::S,  ColorName::rgbDarkGoldenrod},
        {ElementType::Cl, ColorName::rgbLightGreen},
        {ElementType::Br, ColorName::rgbDarkRed},
        {ElementType::I,  ColorName::rgbPurple}
};

inline QColor atomColor(const ElementType &_element) {
    auto itr = colorMap.find(_element);
    if (itr == colorMap.end()) {
        return getQColor(ColorName::rgbBlack);
    }
    return getQColor(itr->second);
}

Mol3DBuilder::Mol3DBuilder(Qt3DCore::QEntity *_rootEntity, std::shared_ptr<JMol> _mol)
        : mRootEntity(_rootEntity), mol3d(std::make_shared<Mol3D>(std::move(_mol))),
          sphereSlices(20), sphereRings(40),
          cylinderSlices(20), cylinderRings(20),
          doubleBondSpaceScale(3), tripleBondSpaceScale(4) {

}

Mol3DBuilder::~Mol3DBuilder() {
}


Qt3DCore::QEntity *Mol3DBuilder::getSphereEntity(
        const QVector3D &_center, const float &_radius, const QColor &_color) {
    auto *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(sphereRings);
    sphereMesh->setSlices(sphereSlices);
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

Qt3DCore::QEntity *Mol3DBuilder::getSingleCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color) {
    auto cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(_radius);
    cylinder->setLength(_from.distanceToPoint(_to));
    cylinder->setRings(cylinderRings);
    cylinder->setSlices(cylinderSlices);

    auto cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.0f);
    // 内置圆柱中轴线在y轴上，重心位于坐标原点
    cylinderTransform->setRotation(QQuaternion::rotationTo(axisY, _from - _to));
    cylinderTransform->setTranslation((_from + _to) / 2.0);

    auto cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(_color);

    auto cylinderEntity = new Qt3DCore::QEntity(mRootEntity);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
    return cylinderEntity;
}


void Mol3DBuilder::reset(std::shared_ptr<JMol> _mol) {
    clear();
    mol3d = std::make_shared<Mol3D>(_mol);
}

void Mol3DBuilder::clear() {
    mol3d = nullptr;// free shared ptr
    for (auto&[_, entity]:mAtomEntities) {
        entity->deleteLater();
    }
    for (auto&[_, entity]:mBondEntities) {
        entity->deleteLater();
    }
    mAtomEntities.clear();
    mBondEntities.clear();
}

Qt3DCore::QEntity *Mol3DBuilder::getDoubleCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color,
        const std::optional<QVector3D> &_normVec) {
    float distance = _radius * doubleBondSpaceScale;
    std::vector<QVector3D> translations = {
            axisZ * distance,
            axisZ * -distance
    };
    return getMultiCylinderEntities(translations, _radius, _color, _from, _to, _normVec);
}

Qt3DCore::QEntity *Mol3DBuilder::getTripleCylinderEntity(
        const QVector3D &_from, const QVector3D &_to,
        const float &_radius, const QColor &_color,
        const std::optional<QVector3D> &_normVec) {
    float distance = _radius * tripleBondSpaceScale;
    std::vector<QVector3D> translations = {
            axisZ * distance,
            zeroP,
            axisZ * -distance,
    };
    return getMultiCylinderEntities(translations, _radius, _color, _from, _to, _normVec);
}

Qt3DCore::QEntity *
Mol3DBuilder::getMultiCylinderEntities(const std::vector<QVector3D> &translations,
                                       const float &_radius, const QColor &_color,
                                       const QVector3D &_from, const QVector3D &_to,
                                       const std::optional<QVector3D> &_normVec) {
    float length = _from.distanceToPoint(_to);
    const size_t entityNum = translations.size();
    std::vector<Qt3DExtras::QCylinderMesh *> cylinderMeshes(entityNum, nullptr);
    for (auto &cylinderMesh:cylinderMeshes) {
        cylinderMesh = new Qt3DExtras::QCylinderMesh();
        cylinderMesh->setRadius(_radius);
        cylinderMesh->setLength(length);
        cylinderMesh->setRings(cylinderRings);
        cylinderMesh->setSlices(cylinderSlices);
    }
    std::vector<Qt3DCore::QTransform *> cylinderTransforms(entityNum, nullptr);
    for (size_t i = 0; i < entityNum; i++) {
        cylinderTransforms[i] = new Qt3DCore::QTransform();
        cylinderTransforms[i]->setScale(1.0f);
        // 内置圆柱中轴线在y轴上，重心位于坐标原点
        // 构造平行圆柱体
        cylinderTransforms[i]->setTranslation(translations[i]);
    }
    std::vector<Qt3DExtras::QPhongMaterial *> cylinderMaterials(entityNum, nullptr);
    for (auto &cylinderMaterial:cylinderMaterials) {
        cylinderMaterial = new Qt3DExtras::QPhongMaterial();
        cylinderMaterial->setDiffuse(_color);
    }
    auto multiCylinderEntity = new Qt3DCore::QEntity(mRootEntity);
    std::vector<Qt3DCore::QEntity *> cylinderEntities(entityNum, nullptr);
    for (size_t i = 0; i < entityNum; i++) {
        cylinderEntities[i] = new Qt3DCore::QEntity(multiCylinderEntity);
        cylinderEntities[i]->addComponent(cylinderMeshes[i]);
        cylinderEntities[i]->addComponent(cylinderMaterials[i]);
        cylinderEntities[i]->addComponent(cylinderTransforms[i]);
    }
    // 将平行圆柱体整体平移、旋转到目标位置
    auto groupTransform = new Qt3DCore::QTransform();
    // 四元数旋转的合成：依次旋转q1、q2 推导为 q2 x q1
    // 键角度调整策略：遍历获取所有需要调整的键，为每个键从三个共面原子计算一个法向量
    QQuaternion rotation = QQuaternion::rotationTo(axisY, _from - _to);
    // FIXME: direction error here
    if (_normVec) {
        auto originVec = rotation * axisX;
        auto &normVec = _normVec.value();
        float dot = QVector3D::dotProduct(normVec, originVec);
        float angle = 180.0 / M_PI * acos(
                dot / originVec.length() / normVec.length());
//        qDebug() << "angle=" << angle;
        rotation *= QQuaternion::fromAxisAndAngle(axisY, angle);
    }
    groupTransform->setRotation(rotation);
    groupTransform->setTranslation((_from + _to) / 2.0);
    multiCylinderEntity->addComponent(groupTransform);
    return multiCylinderEntity;
}

int Mol3DBuilder::getSphereRings() const {
    return sphereRings;
}

void Mol3DBuilder::setSphereRings(int sphereRings) {
    Mol3DBuilder::sphereRings = sphereRings;
}

int Mol3DBuilder::getSphereSlices() const {
    return sphereSlices;
}

void Mol3DBuilder::setSphereSlices(int sphereSlices) {
    Mol3DBuilder::sphereSlices = sphereSlices;
}

int Mol3DBuilder::getCylinderRings() const {
    return cylinderRings;
}

void Mol3DBuilder::setCylinderRings(int cylinderRings) {
    Mol3DBuilder::cylinderRings = cylinderRings;
}

int Mol3DBuilder::getCylinderSlices() const {
    return cylinderSlices;
}

void Mol3DBuilder::setCylinderSlices(int cylinderSlices) {
    Mol3DBuilder::cylinderSlices = cylinderSlices;
}

float Mol3DBuilder::getDoubleBondSpaceScale() const {
    return doubleBondSpaceScale;
}

void Mol3DBuilder::setDoubleBondSpaceScale(float doubleBondSpaceScale) {
    Mol3DBuilder::doubleBondSpaceScale = doubleBondSpaceScale;
}

float Mol3DBuilder::getTripleBondSpaceScale() const {
    return tripleBondSpaceScale;
}

void Mol3DBuilder::setTripleBondSpaceScale(float tripleBondSpaceScale) {
    Mol3DBuilder::tripleBondSpaceScale = tripleBondSpaceScale;
}

bool Mol3DBuilder::build() {
    if(!mol3d)return false;
    if(!mol3d->calcCoord3D_addHs())return false;
    auto mol = mol3d->getMol();
    if (mol->atomsNum() == 0) {
        std::cerr << "get empty molecule in Mol3D::resetMol" << std::endl;
        return false;
    }
    const float baseSize = 200;
    // 坐标系
    QVector3D offset(-baseSize / 3, baseSize / 3, 0);
    getSingleCylinderEntity(offset + zeroP, offset + axisX * 10, 0.2, Qt::blue);
    getSingleCylinderEntity(offset + zeroP, offset + axisY * 10, 0.2, Qt::red);
    getSingleCylinderEntity(offset + zeroP, offset + axisZ * 10, 0.2, Qt::green);
    mol3d->normAtomPosMap3D(baseSize);
    float avgBondLength = 0;
    if (mol->IsBondsEmpty()) {
        avgBondLength = baseSize;
    } else {
        auto getAvgBondLength = [&](const size_t &_bid) -> void {
            auto bond = mol->getBondById(_bid);
            auto &from = mol3d->getAtomPos3DById(bond->getAtomFrom());
            auto &to = mol3d->getAtomPos3DById(bond->getAtomTo());
            avgBondLength += getDistance3D(from, to);
        };
        mol->safeTraverseBonds(getAvgBondLength);
        avgBondLength /= (std::max)((decltype(mol->bondsNum())) (1),
                                    mol->bondsNum());
        avgBondLength = (std::min)(avgBondLength, baseSize);
    }
    auto convertCvPointToQVector3D = [](const cv::Point3f &cvPts) -> QVector3D {
        return QVector3D(cvPts.x, cvPts.y, cvPts.z);
    };
    auto add_atom_entity = [&](const size_t &_aid) -> void {
        auto &ele = mol->getAtomById(_aid)->getElementType();
        auto &pos = mol3d->getAtomPos3DById(_aid);
//        qDebug() << convert(getPos2D);
        mAtomEntities.insert({_aid, getSphereEntity(
                convertCvPointToQVector3D(pos), avgBondLength / 3.5 * atomRadius(ele), atomColor(ele))});
    };
    mol->safeTraverseAtoms(add_atom_entity);
    std::unordered_map<size_t, QVector3D> normVecMap;
    std::unordered_map<size_t, std::vector<size_t>> neighborMap;
    auto get_neighbor_map = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        neighborMap[bond->getAtomFrom()].push_back(bond->getAtomTo());
        neighborMap[bond->getAtomTo()].push_back(bond->getAtomFrom());
    };
    mol->safeTraverseBonds(get_neighbor_map);
    auto find_double_bond_for_rotation = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        if (JBondType::DoubleBond != bond->getBondType()) return;
        std::vector<cv::Point3f> poses;
        std::unordered_set<size_t> aids;
        auto itr1 = neighborMap.find(bond->getAtomFrom());
        if (itr1 != neighborMap.end()) {
            for (auto &aid:itr1->second) {
                if (notExist(aids, aid)) {
                    aids.insert(aid);
                    poses.push_back(mol3d->getAtomPos3DById(aid));
                }
            }
        }
        auto itr2 = neighborMap.find(bond->getAtomTo());
        if (itr2 != neighborMap.end()) {
            for (auto &aid:itr2->second) {
                if (notExist(aids, aid)) {
                    aids.insert(aid);
                    poses.push_back(mol3d->getAtomPos3DById(aid));
                }
            }
        }
        if (poses.size() >= 3) {
            auto p1 = convertCvPointToQVector3D(poses[0]);
            auto p2 = convertCvPointToQVector3D(poses[1]);
            auto p3 = convertCvPointToQVector3D(poses[2]);
            normVecMap[_bid] = QVector3D::crossProduct(p1 - p2, p2 - p3);
        }
    };
    mol->safeTraverseBonds(find_double_bond_for_rotation);
    auto add_bond_entity = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        auto &from = mol3d->getAtomPos3DById(bond->getAtomFrom());
        auto &to = mol3d->getAtomPos3DById(bond->getAtomTo());
        switch (bond->getBondType()) {
            case JBondType::SingleBond:
            case JBondType::DelocalizedBond:
            case JBondType::DashWedgeBond:
            case JBondType::SolidWedgeBond:
            case JBondType::WaveBond: {
                mBondEntities.insert({_bid, getSingleCylinderEntity(
                        convertCvPointToQVector3D(from),
                        convertCvPointToQVector3D(to), avgBondLength / 20,
                        getQColor(ColorName::rgbLightSkyBlue))});
                break;
            }
            case JBondType::DoubleBond: {
                auto itr = normVecMap.find(_bid);
                std::optional<QVector3D> normVec = std::nullopt;
                if (itr != normVecMap.end()) {
                    normVec = itr->second;
                }
                mBondEntities.insert({_bid, getDoubleCylinderEntity(
                        convertCvPointToQVector3D(from),
                        convertCvPointToQVector3D(to), avgBondLength / 30,
                        getQColor(ColorName::rgbLightSkyBlue), normVec)});
                break;
            }
            case JBondType::TripleBond: {
                auto itr = normVecMap.find(_bid);
                std::optional<QVector3D> normVec = std::nullopt;
                if (itr != normVecMap.end()) {
                    normVec = itr->second;
                }
                mBondEntities.insert({_bid, getTripleCylinderEntity(
                        convertCvPointToQVector3D(from),
                        convertCvPointToQVector3D(to), avgBondLength / 40,
                        getQColor(ColorName::rgbIndianRed), normVec)});
                break;
            }
            default: {
                std::cerr << "unhandled bond type: "
                          << static_cast<int>(bond->getBondType()) << std::endl;
                exit(-1);
            }
        }
    };
    mol->safeTraverseBonds(add_bond_entity);
    return true;//success
}
