/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef IOUTIL_H
#define IOUTIL_H

#include "cocr.h"

#include <QString>
#include <QByteArray>

class COCR_EXPORTS IOUtil {
public:
	IOUtil();
	/*递归释放_qrcTopDir目录下所有文件至_targetDir目录，不保留原始目录结构*/
	static bool release(const QString& _qrcTopDir, const QString& _targetDir);
	/*按文件名字典序拼接分片文件*/
	static const QByteArray getSplitFilesUnder(const QString& _qrcTopDir);
	/*打印目录树*/
	static void print(const QString& _qrcTopDir);
	/*返回语言列表*/
	static const QStringList getLanguageList(const QString& _langDir = ":/Resources/lang/");
};

#endif  // IOUTIL_H
