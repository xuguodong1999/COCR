/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include "cvutil.h"
#include "topo.h"
#include <vector>
class Synthesizer {
public:
	Synthesizer();
	void collect(
		const cv::Mat& frame,				//输入：对应于rects的像素图
		const std::vector<RectObj>& rects,	//输入：检出目标
		ItemTopo& itemTopo					//输出：连接方式
	);
private:
	bool IsAtom(int label);
	const char* label2name(int label);
	void collect_XgdsImplement(
		std::vector<int>& aids,
		const std::vector<int>& bids,
		std::vector<int>& aindices,
		const std::vector<int>& bindices,
		std::vector<QPointF>& aposes,
		const std::vector<QLineF>& bposes,
		std::vector<std::vector<int>>& cons,
		const cv::Mat& frame,
		const std::vector<RectObj>& rects,
		ItemTopo& items
	);
};

#endif // SYNTHESIZER_H
