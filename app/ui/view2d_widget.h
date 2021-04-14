#ifndef VIEW2D_WIDGET_H
#define VIEW2D_WIDGET_H

#include <QWidget>

namespace Ui {
    class View2DWidget;
}
namespace xgd {
    class JMol;
}
class Mol2DWidget;

class View2DWidget : public QWidget {
Q_OBJECT
public:
    explicit View2DWidget(QWidget *parent = nullptr);

    ~View2DWidget();

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

    void startWaitHint();

private:
    Ui::View2DWidget *ui;
    Mol2DWidget *mol2DWidget;
};

#endif // VIEW2D_WIDGET_H
