package com.xgd.axxlab.detector;

import android.graphics.Color;
import android.graphics.Paint;

public class Config {
    static public final int SIZE_PER_PIX = 3;   // 模型要求的图像输入通道
    static public final int DOWN_RATIO = 32;    // 模型最大下采样倍数
    static public final int SIZE_FLOAT = 4;     // 浮点数占位
    static public final int CLASS_NUM = 11;     // 类别数
    static public final int HEAD_NUM = 3;       // 检测头个数
    static public final int ANCHOR_NUM = 3;     // 锚框个数
    static public final int LOC_PARM_NUM = 5;   // 使用几个非分类参数
    static public final int INPUT_WIDTH = 800;  // 输入尺寸
    static public final int INPUT_HEIGHT = 448; // 输入尺寸

    // 相对assets目录的模型路径
    static public final String ModelFileName = "soso-11-f16-800x448.tflite";
    // 分类标签
    static public final String[] SosoLabels = {
            //"Dash","Single","Double","Triple","Solid","C","H","O","N","P","S"
            //0   1    2    3    4     5    6    7    8    9    10
            "@", "-", "=", "#", "@@", "C", "H", "O", "N", "P", "S"};

    // 理想检测尺寸
    static public final float IDEAL_CHAR_SIZE = 50;
    // 用于检测器输入的重绘画笔
    static public final Paint DetectorPainter;

    static {
        DetectorPainter = new Paint(Paint.ANTI_ALIAS_FLAG);
        DetectorPainter.setColor(Color.BLACK);
        DetectorPainter.setStrokeWidth(3);
        DetectorPainter.setStrokeCap(Paint.Cap.ROUND);
        DetectorPainter.setStrokeJoin(Paint.Join.ROUND);
    }
}
