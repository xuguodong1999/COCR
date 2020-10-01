package com.xgd.axxlab.detector;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.PointF;

import java.util.List;

public interface Detector {
    int getThreadNum();

    /**
     * 调用之后必须再调用 initialize 方可生效
     *
     * @param threadNum 线程数
     */
    void setThreadNum(int threadNum);

    /**
     * 加载模型，需要传入一个 AssetManager，getAssets() 获得
     *
     * @param assetManager AssetManager
     * @return 是否初始化成功
     */
    boolean initialize(AssetManager assetManager);

    /**
     * 释放掉模型占用的所有内存，后面使用需要先调用 initialize
     */
    void close();

    boolean IsOptimizeEnabled();

    /**
     * 调用之后必须再调用 initialize 方可生效
     *
     * @param isOptimizeEnabled 是否使用 Optimize
     */
    void enableOptimize(boolean isOptimizeEnabled);

    float getIOUThresh();

    void setIOUThresh(float thresh);

    float getConfThresh();

    void setConfThresh(float thresh);

    /**
     * 输入的位图尺寸必须满足 Config 中的 INPUT_WIDTH x INPUT_HEIGHT，现在 tflite 不支持动态尺寸
     *
     * @param bitmap
     * @return
     */
    List<Recognition> getRecognition(Bitmap bitmap);

    /**
     * 输入任意分辨率的笔迹序列，含义：List<笔画>，笔画<=>List<PointF>
     *
     * @param script 笔迹序列
     * @return
     */
    List<Recognition> getRecognition(List<List<PointF>> script);
}
