/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef OBUTIL_H
#define OBUTIL_H

#include "cocr.h"

#include <QApplication>
#include <QString>
#include <QByteArray>
#include <string>

class COCR_EXPORTS OBUtil {
public:
	OBUtil();
	std::string getpdb(const char* smiles);
	// private:
	bool releasePlugin();
	const QByteArray obPluginPath = ":/obabel";
	const QByteArray obPluginFilePath = getAppDirPath().toUtf8() + "/obabel";
};

#endif  // OBUTIL_H
