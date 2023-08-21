#pragma once

#include <QWidget>

namespace Ui {
    class MainTabWidget;
}
class WelcomeWidget;

class PaintWidget;

class View2DWidget;

#ifdef QT_3DCORE_LIB

class View3DWidget;

#endif

class OCRThread;

class QHBoxLayout;

class MainTabWidget : public QWidget {
Q_OBJECT

    Ui::MainTabWidget *ui;
    WelcomeWidget *welcomeWidget;
    PaintWidget *paintWidget;
    View2DWidget *view2DWidget;
#ifdef QT_3DCORE_LIB
    View3DWidget *view3DWidget;
#endif
    OCRThread *ocrThread;
    bool is2DLastUsed;
    bool isMolLatest;
    bool isOCRBtnClicked;
    bool isAgreementChecked;
    bool isFromDraw;
    enum class DataSource {
        CAMERA, IMAGE, PAINT
    } lastSource;
    const char *KEY_IS_2D_LAST_USED = "main_tab_widget/is_2d_last_used";
public:
    explicit MainTabWidget(QWidget *parent = nullptr);

    ~MainTabWidget() override;

protected:
    void resizeEvent(QResizeEvent *e) override;

    void closeEvent(QCloseEvent *e) override;

private Q_SLOTS:

    void setAgreementChecked(bool isChecked);

    void handleTabChange(int index);

    void onOcrJobReady();

    void doOCR(const QList<QList<QPointF>> &_script);

    void doOCR(const QImage &_image);

private:
    // only keep items foreground to save memory
    QHBoxLayout *v2dLayout, *v3dLayout, *camLayout;

    void safeAttach2DWidget();

    void safeDelete2DWidget();

    void onMolReady();

#ifdef QT_3DCORE_LIB

    void safeAttach3DWidget();

    void safeDelete3DWidget();

#endif

    void setRecentlyUsedViewer(bool is2D);
};