/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef CVUTIL_H
#define CVUTIL_H

#include "cocr.h"

#include <opencv2/dnn.hpp>

#include <QImage>
#include <QRectF>

#include <map>
#include <vector>
#include <iostream>

class COCR_EXPORTS RectObj {
public:
	RectObj();
	~RectObj();
	QRectF outline;
	int label;
	float possibility;
	friend std::ostream& operator<<(std::ostream& out, const RectObj& obj);
};

class COCR_EXPORTS CVUtil {
public:
	CVUtil();
	bool init(
		const char* weightsPath = ":/weights/enetb0.weights",
		const char* cfgPath = ":/weights/enetb0.cfg",
		const char* namelistPath = nullptr
	);
	void detectYolov3(const QImage& image, int topk = 1, float conf_thresh = 0.2,
		float iou_thresh = 0.3, bool isGaussian = false);
	const cv::Mat getFrame();
	const std::vector<RectObj>& getObjs();
	static QLineF getLineDirection(const cv::Mat& image3c, const QRectF& qrect);
private:
	std::vector<RectObj> objs;
	std::vector<std::string> names;
	cv::dnn::Net net;
	cv::Mat frame, blob;
	double getAverageYoloBoxWidth(const cv::Mat& frame, const std::vector<cv::Mat>& outs, int bias, double conf_thresh = 0.5);
};

#endif // CVUTIL_H
