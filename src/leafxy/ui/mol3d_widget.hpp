#pragma once
//#include "gesture_widget.hpp"
#include <QWidget>

class Mol3DWindow;

class Mol3DBuilder;
namespace Qt3DCore {
    class QEntity;
}
class WaitHintWidget;
namespace cocr {
    class JMol;
}
// Qt3DWindow 无法生成 QGestureEvent
class Mol3DWidget : public QWidget {
Q_OBJECT
    std::shared_ptr<cocr::JMol> mol;
    Mol3DWindow *window;
    QWidget *mol3DWindowContainer;
    WaitHintWidget *hintWidget;
    Qt3DCore::QEntity *root;
    Mol3DBuilder *builder;
    float minViewWidth;
public:
    Mol3DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<cocr::JMol> _mol);

    void startWaitHint();

    QString makeAtomInfo(const size_t &_aid);

    QString makeBondInfo(const size_t &_bid);

protected:
    void resizeEvent(QResizeEvent *e) override;

public slots:

    void endWaitHint();
//    bool eventFilter(QObject *watched, QEvent *e);
};