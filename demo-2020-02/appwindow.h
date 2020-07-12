/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "cocr.h"
#include "cvutil.h"
#include <QMainWindow>

namespace Ui {
	class AppWindow;
}
#include <QVector>
class COCR_EXPORTS AppWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit AppWindow(QWidget* parent = nullptr);
	~AppWindow();
protected:
	void closeEvent(QCloseEvent* e)override;
	void resizeEvent(QResizeEvent* e)override;
private:
	Ui::AppWindow* ui;
	CVUtil cvutil;
public slots:
	void setHWidget(QVector<QWidget*>& w);
	void setVWidget(QVector<QWidget*>& w);
private slots:
	void setLanguage(const QString& language);
	void about();
	void aboutQt();
	void openImage();
};

#endif // APPWINDOW_H
