/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "cocr.h"
#include <QLayout>
#include <QScrollArea>
#include <QImage>
#include <QPixmap>
#include <QLabel>
void closeLayout(QLayout* l) {
	QLayoutItem* child;
	while ((child = l->takeAt(0)) != nullptr) {
		if (child->widget() != nullptr) {
			delete child->widget();
		}
		else if (child->layout() != nullptr) {
			closeLayout(child->layout());
		}
		delete child;
	}
	delete l;
}
QScrollArea* getImageViewer(QImage* image, QSize size) {
	auto s = new QScrollArea();
	auto w = new QWidget();
	auto label = new QLabel(w);
	label->setPixmap(
		QPixmap::fromImage(*image).scaled(
			size,
			Qt::KeepAspectRatio,
			Qt::FastTransformation
		)
	);
	w->setMinimumSize(size);
	s->setWidget(w);
	return s;
}

QString getAppDirPath() { return QApplication::applicationDirPath(); }

void setAppAttribute() {
	qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
	qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
	qApp->setApplicationName(QObject::tr("COCR"));
	qApp->setOrganizationName(QObject::tr("NJU"));
	qApp->setApplicationVersion(QObject::tr("2020.2.16"));
	qApp->setApplicationDisplayName(QObject::tr("COCR"));
#ifdef USE_QT_ANDROID
	getAndroidPermission();
#endif
}

void setAppTranslator(const QString& language) {
	static QTranslator qt_translator;
	const QString appqm = ":/Resources/lang/app_" + language + ".qm";
	if (!qt_translator.load(appqm)) {
		qDebug() << "Missing app qm file:(" << appqm << "). Use default qm file.";
		const QString qtqm = ":/Resources/lang/qt_" + language + ".qm";
		if (!qt_translator.load(qtqm)) {
			qDebug() << "Missing default qm file: (" << qtqm << ").";
		}
		else {
			qApp->installTranslator(&qt_translator);
		}
	}
	else {
		qApp->installTranslator(&qt_translator);
	}
}
void moveToCenter(QWidget& _widget, double _w_scale, double _h_scale) {
	assert(_w_scale <= 1 && _h_scale <= 1);
	QSize size = QApplication::desktop()->size();
	int w = _w_scale * size.width(),
		h = _h_scale * size.height();
	_widget.resize(w, h);
	int x = (size.width() - w) / 2,
		y = (size.height() - h) / 2;
	_widget.move(x, y);
}

#ifdef USE_QT_ANDROID
#include <QtAndroid>
#endif // USE_QT_ANDROID
bool getAndroidPermission() {
#ifdef USE_QT_ANDROID
	QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
	if (r == QtAndroid::PermissionResult::Denied) {
		QtAndroid::requestPermissionsSync(QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE");
		r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
		if (r == QtAndroid::PermissionResult::Denied) {
			return false;
		}
	}
	return true;
#else
	return false;
#endif // USE_QT_ANDROID
}
