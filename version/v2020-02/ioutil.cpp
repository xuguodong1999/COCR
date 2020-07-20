/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "ioutil.h"
#include <QDebug>
#include <QFile>
#include <QDir>

IOUtil::IOUtil() {}

bool IOUtil::release(const QString& _qrcTopDir, const QString& _targetDir) {
	QDir qrcTopDir(_qrcTopDir);
	if (!qrcTopDir.exists()) {
		qDebug() << "!qrcTopDir.exists()";
		return false;
	}
	QDir targetDir(_targetDir);
	if (!targetDir.exists()) {
		if (!targetDir.mkpath(_targetDir)) {
			qDebug() << "!targetDir.mkpath(_targetDir)";
			return false;
		}
	}
	QFileInfoList fileInfoList = qrcTopDir.entryInfoList();
	while (!fileInfoList.isEmpty()) {
		QFileInfo tempFileInfo = fileInfoList.last();
		if (tempFileInfo.isFile()) {
			const QString newFilePath = _targetDir + "/" + tempFileInfo.fileName();
			if (!QFile::exists(newFilePath)) {
				QFile::copy(tempFileInfo.absoluteFilePath(), newFilePath);
			}
			if (!QFile::exists(newFilePath)) {
				qDebug() << "!QFile::exists(newFilePath)";
				return false;
			}
			fileInfoList.removeLast();
		}
		else if (tempFileInfo.isDir()) {
			if (tempFileInfo.fileName() != "." && tempFileInfo.fileName() != "..") {
				QDir subDir(tempFileInfo.filePath());
				fileInfoList.removeLast();
				fileInfoList.append(subDir.entryInfoList());
			}
		}
		else {
			fileInfoList.removeLast();
		}
	}
	return true;
}

const QByteArray IOUtil::getSplitFilesUnder(const QString& _qrcTopDir) {
	QByteArray result;
	QDir qrcTopDir(_qrcTopDir);
	if (!qrcTopDir.exists()) {
		qDebug() << "!qrcTopDir.exists()";
		return result;
	}
	QFileInfoList fileInfoList = qrcTopDir.entryInfoList();
	for (const auto& file : fileInfoList) {
		if (file.isFile()) {
			QFile f(file.absoluteFilePath());
			f.open(QIODevice::ReadOnly);
			result.append(f.readAll());
			f.close();
		}
	}
	return result;
}
void IOUtil::print(const QString& _qrcTopDir) {
	QDir qrcTopDir(_qrcTopDir);
	QFileInfoList fileInfoList = qrcTopDir.entryInfoList();
	while (!fileInfoList.isEmpty()) {
		QFileInfo tempFileInfo = fileInfoList.last();
		qDebug() << tempFileInfo.absoluteFilePath();
        if (tempFileInfo.isDir()) {
			if (tempFileInfo.fileName() != "." && tempFileInfo.fileName() != "..") {
				QDir subDir(tempFileInfo.filePath());
				fileInfoList.removeLast();
				fileInfoList.append(subDir.entryInfoList());
			}
		}
		else {
			fileInfoList.removeLast();
		}
	}
}
const QStringList IOUtil::getLanguageList(const QString& _langDir) {
	QStringList result;
	QDir qrcTopDir(_langDir);
	if (!qrcTopDir.exists()) {
		qDebug() << "!qrcTopDir.exists()";
		return result;
	}
	QFileInfoList fileInfoList = qrcTopDir.entryInfoList();
	for (const auto& file : fileInfoList) {
		QString fn = file.fileName();
		if (fn.contains(QRegExp("qt_(.*).qm"))) {
			result << fn.remove("qt_").remove(".qm");
		}
	}
	return result;
}

