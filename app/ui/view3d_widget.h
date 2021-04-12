#ifndef VIEW3D_WIDGET_H
#define VIEW3D_WIDGET_H

#include <QWidget>

namespace Ui {
    class View3DWidget;
}
namespace xgd {
    class JMol;
}
class Mol3DWidget;

class View3DWidget : public QWidget {
Q_OBJECT

public:
    explicit View3DWidget(QWidget *parent = nullptr);

    ~View3DWidget();

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

private:
    Ui::View3DWidget *ui;
    Mol3DWidget *mol3DWidget;
};

#endif // VIEW3D_WIDGET_H
