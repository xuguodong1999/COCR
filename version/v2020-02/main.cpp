/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "appwindow.h"
#include <QApplication>
#include "obutil.h"
#include "ioutil.h"
void sosoTest();
int  main(int argc, char* argv[]) {
	setAppAttribute();
	QApplication app(argc, argv);
    IOUtil::print(":/");
	OBUtil ob;
	ob.releasePlugin();
//	std::string s;
//	while (getline(std::cin, s)) {
//		std::cout << ob.getpdb(s.c_str()) << std::endl;
//	}
//	exit(-1);


	if (QLocale().language() == QLocale::Chinese) {
		setAppTranslator("zh_CN");
	}
	//sosoTest();
	(new AppWindow)->showMaximized();
#ifdef USE_QT_ANDROID
	getAndroidPermission();
#endif // USE_QT_ANDROID
	
	qDebug() << "Enter Event Loop...";
	return app.exec();
}

#include "cvutil.h"
using namespace std;
void sosoTest() {
	CVUtil du;
	QImage img;
	char   s[512];
	cout << "input:";
	while (cin >> s) {
		if (!img.load(s))
			if (!img.load(QString("E:/") + s + QString(".jpg")))
				break;
		time_t start = clock();
		du.detectYolov3(img.convertToFormat((QImage::Format::Format_RGB888)));
		qDebug() << "detect costs" << (double)(clock() - start) * 1000.0 / (double)CLOCKS_PER_SEC << " ms";
		for_each(du.getObjs().begin(), du.getObjs().end(), [=](const auto& i) { cout << i << endl; });
		cout << "input:";
	}
}
