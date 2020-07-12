/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef COLORUTIL_H
#define COLORUTIL_H

#include "cocr.h"

#include <vector>
using std::vector;
#include <opencv2/opencv.hpp>
using cv::Scalar;
using cv::Mat;
#include <QImage>

class COCR_EXPORTS ColorUtil
{
public:
	ColorUtil();
	static Scalar getColor(int index, int total);
	static Mat qimage2mat(const QImage& image);
private:
	static vector<vector<float>> colorTable;

};
#endif // COLORUTIL_H
