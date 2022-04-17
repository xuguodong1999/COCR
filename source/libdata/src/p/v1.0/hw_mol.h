#pragma once


#include <string>
#include <vector>
#include <map>
#include "stroke/controller.h"
#include "stroke/abs.h"
#include "mol_holder.h"

namespace v1_0 {

/**
 * 向JMol填充图元
 */
    class HwMol : public HwBase, public MolHolder {
        std::vector<std::shared_ptr<HwBase>> mData;
        HwController *hwController;
        std::shared_ptr<MolHolder> mol2dHolder, molOpHolder;
        std::unordered_map<size_t, size_t> hwToAtomMap;
        float avgSize;

        /**
         * @param _explicitCarbonProb 控制碳原子是否显示写出
         * @return 平均图元大小
         */
        float reloadHWData(const float &_explicitCarbonProb = 0.1);

        /**
         * 检查所有字符的周边环境，如果满足嵌入要求，将字符图元替换为字符串图元
         * 替换策略为随机，满足低交叠原则
         * 在 paintTo 函数调用前调用之，调用该函数后需要重新计算画布位置
         * @param _prob 替换为字符串图元的概率
         */
        void replaceCharWithText(const float &_prob = 0.5);

    public:
        static std::shared_ptr<HwMol> GetSpecialExample(float _explicitCarbonProb);

        std::shared_ptr<HwBase> clone() const override;

        void setHwController(HwController &_hwController) override;

        void showOnScreen(const size_t &_repeatTimes = 1, bool _showBox = false);

        void showSpecialExample(const size_t &_repeatTimes = 1, bool _showBox = false);

        /**
         *
         * @param _imgPath C:/soso/JPEGImages/1a43, e.g. index will append as _i.jpg
         * @param _labelPath C:/soso/labels/1a43, e.g. index will append as _i.txt
         * @param _repeatTimes
         */
        void dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                           const size_t &_repeatTimes = 1);

        /**
         * 用 JMol 构造一个几何分子类型
         * @param _jmol Construct a MolItem from JMol
         */
        HwMol(std::shared_ptr<MolHolder> _molOpHolder, std::shared_ptr<MolHolder> _mol2dHolder = nullptr,
              HwController *_hwController = nullptr);

        void paintTo(Mat &_canvas) const override;

        std::optional<rectf> getBoundingBox() const override;

        void rotate(float _angle) override;

        void rotateBy(float _angle, const point2f &_cent) override;

        void moveBy(const point2f &_offset) override;

        void mulK(float _kx, float _ky) override;

    };
}