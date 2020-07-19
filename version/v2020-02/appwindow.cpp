/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "ioutil.h"
#include "scene.h"
#include "view.h"
#include "colorutil.h"
#include "appwindow.h"
#include "topo.h"
#include "synthesizer.h"

#include "ui_appwindow.h"
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QFileInfo>
#include <QFileDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QScrollArea>

AppWindow::AppWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::AppWindow)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	//语言
	connect(ui->actionChinese, &QAction::triggered, this, [=]() {setLanguage("zh_CN"); });
	connect(ui->actionEnglish, &QAction::triggered, this, [=]() {setLanguage("en"); });
	//关于
	connect(ui->actionAbout_Qt, &QAction::triggered, this, &AppWindow::aboutQt);
	connect(ui->actionAbout, &QAction::triggered, this, &AppWindow::about);
	//打开
	connect(ui->actionImage, &QAction::triggered, this, &AppWindow::openImage);
	connect(ui->toolButton, &QToolButton::clicked, this, &AppWindow::openImage);
	//调试
	connect(ui->actionDebugConsole, &QAction::triggered, this,
		[=]() {
			// 验证删除窗体布局，无内存泄漏
			//for (int i = 0; i < 100; i++) {
			//	QVector<QWidget*>w;
			//	for (int i = 0; i < 100; i++) {
			//		w.push_back(new AppWindow);
			//	}
			//	setHWidget(w);
			//}
			//验证图形视图框架操作，手动生成拓扑
			//qDebug() << "test graphics: (NH2)CHO inited";
			//Scene* scene = new Scene();
			//ItemTopo mol;
			//int a1 = mol.insertAtom("Atom", "H", QPointF(50, 50));
			//int a2 = mol.insertAtom("Atom", "H", QPointF(175, 25));
			//int a3 = mol.insertAtom("Atom", "H", QPointF(175, 75));
			//int a4 = mol.insertAtom("Atom", "N", QPointF(150, 50));
			//int a5 = mol.insertAtom("Atom", "C", QPointF(100, 100));
			//int a6 = mol.insertAtom("Atom", "O", QPointF(100, 150));
			//int b1 = mol.insertBond("SingleBond");
			//int b2 = mol.insertBond("SingleBond");
			//int b3 = mol.insertBond("DoubleBond");
			//int b4 = mol.insertBond("SolidBond");
			//int b5 = mol.insertBond("DashBond");
			//mol.setFromTo(b1, a3, a4);
			//mol.setFromTo(b2, a2, a4);
			//mol.setFromTo(b3, a5, a6);
			//mol.setFromTo(b4, a5, a1);
			//mol.setFromTo(b5, a5, a4);
			//mol.setToScene(scene);
			//View* view = new View();
			//view->setScene(scene);
			//setCentralWidget(view);
			//验证结构综合，图形界面调试
			//QString fpath = getAppDirPath() + "/../../testcase/26b9.jpg";
			//qDebug() << "test synthesizer: " << fpath << " inited";
			//ItemTopo it;
			//Synthesizer s;
			//QImage image;
			//if (!image.load(fpath))return;
			//cvutil.detectYolov3(image.convertToFormat((QImage::Format::Format_RGB888)));
			//const cv::Mat& mat = cvutil.getFrame();
			//auto& rects = cvutil.getObjs();
			//s.collect(mat, rects, it);
			//Scene* scene = new Scene();
			//it.setToScene(scene);
			//View* view = new View();
			//view->setScene(scene);
			//setCentralWidget(view);
		}
	);
}

AppWindow::~AppWindow() {
	//qDebug() << this<<"::~AppWindow";
	delete ui;
}
static View* last = nullptr;
void AppWindow::openImage() {
	auto process = [=](QImage& image) {
		cvutil.detectYolov3(image.convertToFormat((QImage::Format::Format_RGB888)));
		const cv::Mat& mat = cvutil.getFrame();
		auto& rects = cvutil.getObjs();
		ItemTopo it;
		Synthesizer s;
		s.collect(mat, rects, it);
		Scene* scene = new Scene();
		it.setToScene(scene);
		View* view = new View();
		view->setScene(scene);
		if (last != nullptr)
			ui->horizontalLayout->removeWidget(last);
		ui->horizontalLayout->addWidget(view, 5);
		last = view;
		QMessageBox(QMessageBox::NoIcon, tr("Caution"), tr("COCR is still UNDER development. Bug flies.")).exec();
	};
#if defined(USE_QT_ANDROID) && (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	auto fileContentReady = [=](const QString& fileName, const QByteArray& fileContent) {
		if (fileName.isEmpty()) {
			qDebug() << "Fail to find " << fileName;
		}
		else {
			// Use fileName and fileContent
			QImage image = QImage::fromData(fileContent);
			if (!image.isNull()) {
				process(image);
			}
			else {
				qDebug() << "QFileDialog::getOpenFileContent: Fail to load " << fileName;
			}
		}

	};
	QFileDialog::getOpenFileContent(
		tr("Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)"),
		fileContentReady
	);
#else
	QDir dir("/storage/emulated/0/");
	QString dirPath;
	if (!dir.exists())
		dirPath = getAppDirPath();
	else {
		dirPath = dir.path();
	}
	QString imgPath = QFileDialog::getOpenFileName(
		this,
		tr("Choose an image file"),
		dirPath,
		tr("Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)")
	);
	QImage image;
	if (!imgPath.isNull()) {
		if (image.load(imgPath)) {
			process(image);
		}
		else {
			qDebug() << "Fail to load " << imgPath;
		}
	}
	else {
		qDebug() << "Fail to find " << imgPath;
	}
#endif
}

void AppWindow::closeEvent(QCloseEvent* e) {
	//qDebug() << this<<"::closeEvent";
	QMainWindow::closeEvent(e);
}

void AppWindow::resizeEvent(QResizeEvent* e) {
	QMainWindow::resizeEvent(e);
}

void AppWindow::setHWidget(QVector<QWidget*>& w) {
	QLayout* l = centralWidget()->layout();
	if (l != nullptr) {
		closeLayout(l);
	}
	QHBoxLayout* hl = new QHBoxLayout();
	for (int i = 0; i < w.size(); i++) {
		hl->insertWidget(i, w[i], 1);
	}
	centralWidget()->setLayout(hl);
}
void AppWindow::setVWidget(QVector<QWidget*>& w) {
	QLayout* l = centralWidget()->layout();
	if (l != nullptr) {
		closeLayout(l);
	}
	QVBoxLayout* vl = new QVBoxLayout();
	for (int i = 0; i < w.size(); i++) {
		vl->insertWidget(i, w[i], 1);
	}
	centralWidget()->setLayout(vl);
}
void AppWindow::setLanguage(const QString& language) {
	setAppTranslator(language);
	ui->retranslateUi(this);
}
void AppWindow::aboutQt() {
	QMessageBox::aboutQt(this);
}
void AppWindow::about() {
	const QString text = tr(
		"<h3>About COCR</h3>"
	);
	const QString info = tr(
		"<p>COCR is a cross-platform Chemical OCR application.</p>"
		"<p>COCR mainly provides OCR capacity for handwritting and print-out images"
		" of structure formulas in chemistry.</p>"
		"<p>COCR aims to become a ready-to-use editor for chemistry-related work.</p>"
		"<p>Copyright (C) %1 Guodong Xu, Shaoheng Cao, Siwen Chen and QiJia Dai.</p>"
		"<p>COCR is a product of Undergraduate Innovation Training Program in Software Institute of Nanjing University. See <a href=\"https://%2/\">%2</a> for more information.</p>"
	).arg(2020).arg("software.nju.edu.cn");

	QMessageBox* msgBox = new QMessageBox(this);
	msgBox->setAttribute(Qt::WA_DeleteOnClose);
	msgBox->setWindowTitle(tr("About COCR"));
	msgBox->setText(text);
	msgBox->setInformativeText(info);

	QPixmap pm(QLatin1String(":/Resources/images/cocr.png"));
	if (!pm.isNull()) {
		msgBox->setIconPixmap(pm.scaled(256, 256));
	}
	msgBox->exec();
}
