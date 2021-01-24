#include "mol3d.hpp"
#include "mol3d_builder.hpp"
#include "mol3dwindow.hpp"
#include "openbabel_util.hpp"
#include <QApplication>
#include <QWidget>
#include <Qt3DCore/QEntity>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[]) {
    std::srand(0);
#ifdef Q_OS_WIN64
    _putenv("BABEL_DATADIR=C:/static/openbabel-3.1.1/data");
#endif
    QApplication a(argc, argv);
    auto molUtil = std::make_shared<MolUtilOpenBabelImpl>();
    //C1C(C=C)CC(C#C)C(C#C)C1
    //[H]OC1=CC=CC([C@](C2=CC([H])=CC([H])=C2)(C2=CC(C)=C([H])N=C2)[C@@](Cl)(C2=CC([H])=C[P@@H]2)C([H])([H])O[H])=N1
    auto mol = molUtil->fromFormat("[H]OC1=CC=CC([C@](C2=CC([H])=CC([H])=C2)(C2=CC(C)=C([H])N=C2)[C@@](Cl)(C2=CC([H])=C[P@@H]2)C([H])([H])O[H])=N1", "smi");
    auto rootEntity = new Qt3DCore::QEntity();
    auto molBuilder = new Mol3DBuilder(rootEntity, mol);
    if (!molBuilder->build())exit(-1);
    auto view = new Mol3DWindow(rootEntity);
    auto container = QWidget::createWindowContainer(view);
    container->showMaximized();
//    qDebug() << mol->atomsNum() << "," << mol->bondsNum();
//    std::cout << molUtil->getFormat(*mol, "pdb") << std::endl;
    return a.exec();
}