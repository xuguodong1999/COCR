package com.xgd.axxlab.detector;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PointF;
import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import static com.xgd.axxlab.detector.Config.DetectorPainter;
import static com.xgd.axxlab.detector.Config.IDEAL_CHAR_SIZE;
import static com.xgd.axxlab.detector.Config.INPUT_WIDTH;
import static com.xgd.axxlab.detector.Config.SIZE_FLOAT;
import static com.xgd.axxlab.detector.Config.SIZE_PER_PIX;


public class Utils {
    static final String TAG = "dnn.Utils";

    /**
     * 把笔迹点缩放到合适尺度。注意：script的内容会被改变！
     *
     * @param script List<List<PointF>> 原始笔迹
     * @return List<List < PointF>> 缩放后的笔迹
     */
    static public List<List<PointF>> normalizeScript(List<List<PointF>> script) {

        float whiteSpaceX = 10, whiteSpaceY = 10;
        float targetWidth = INPUT_WIDTH - whiteSpaceX;
        float targetHeight = INPUT_WIDTH - whiteSpaceY;
        float k = 1;// 缩放比例

        float minx, miny, maxx, maxy,
                minx0, miny0, maxx0, maxy0;
        minx = miny = Float.MAX_VALUE;
        maxx = maxy = Float.MIN_VALUE;
        List<Float> sizeList = new ArrayList<>();
        for (List<PointF> stroke : script) {
            minx0 = miny0 = Float.MAX_VALUE;
            maxx0 = maxy0 = Float.MIN_VALUE;
            for (PointF pts : stroke) {
                minx0 = Math.min(pts.x, minx0);
                miny0 = Math.min(pts.y, miny0);
                maxx0 = Math.max(pts.x, maxx0);
                maxy0 = Math.max(pts.y, maxy0);
            }
            sizeList.add(maxx0 - minx0);
            sizeList.add(maxy0 - miny0);
            minx = Math.min(minx0, minx);
            miny = Math.min(miny0, miny);
            maxx = Math.max(maxx0, maxx);
            maxy = Math.max(maxy0, maxy);
        }
        Collections.sort(sizeList);
        float avgSize = 0;
        int begin = sizeList.size() / 3, end = sizeList.size() / 3 * 2;
        for (int i = begin; i < end; i++) {
            avgSize += sizeList.get(i);
        }
        avgSize /= Math.max(end - begin, 1);
        if (script.size() > 4) {
            k = IDEAL_CHAR_SIZE / avgSize;
        }
        // else: 笔画太少，怀疑连笔为主，放弃调整比例
        final float currentWidth = maxx - minx;
        final float currentHeight = maxy - miny;
        // 缩放到理想尺度，但是不能超过画布大小
        if (k * currentHeight > targetHeight) {
            k = targetHeight / currentHeight;
        }
        if (k * currentWidth > targetWidth) {
            k = targetWidth / currentWidth;
        }
        Log.d(TAG, "k=" + k + ",LIST=" + sizeList);
        targetHeight = k * currentHeight;
        targetWidth = k * currentWidth;
        whiteSpaceX = (INPUT_WIDTH - targetWidth) / 2;
        whiteSpaceY = (INPUT_WIDTH - targetHeight) / 2;
        for (List<PointF> stroke : script) {
            for (PointF pts : stroke) {
                pts.x = (pts.x - minx) * k + whiteSpaceX;
                pts.y = (pts.y - miny) * k + whiteSpaceY;
            }
        }
        return script;
    }

    /**
     * 把笔迹点转换为像素图
     *
     * @param script List<List<PointF>> 笔迹
     * @return Bitmap SIZE_FIXED_INPUT x SIZE_FIXED_INPUT x 3
     */
    static public Bitmap convertScriptToBitmap(List<List<PointF>> script) {
        Bitmap bitmap = Bitmap.createBitmap(INPUT_WIDTH, INPUT_WIDTH,
                Bitmap.Config.RGB_565, false);
        bitmap.eraseColor(Color.WHITE);
        Canvas canvas = new Canvas(bitmap);
        for (List<PointF> stroke : script) {
            if (stroke.size() == 0) {
                continue;
            }
            canvas.drawLine(stroke.get(0).x, stroke.get(0).y,
                    stroke.get(0).x, stroke.get(0).y, DetectorPainter);
            for (int i = 1; i < stroke.size(); i++) {
                canvas.drawLine(stroke.get(i).x, stroke.get(i).y,
                        stroke.get(i - 1).x, stroke.get(i - 1).y, DetectorPainter);
            }
        }
        return bitmap;
    }

    /**
     * 把原图转化为模型输入，丢弃右下侧 size % 32 的部分
     *
     * @param bitmap
     * @return ByteBuffer modelInput
     */
    static public ByteBuffer convertBitmapToByteBuffer(Bitmap bitmap) {
        final int oldWidth = bitmap.getWidth();
        final int oldHeight = bitmap.getHeight();
        final int newWidth = oldWidth - oldWidth % 32;
        final int newHeight = oldHeight - oldHeight % 32;
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(
                SIZE_FLOAT * newWidth * newHeight * SIZE_PER_PIX).order(ByteOrder.nativeOrder());
        int[] intBuffer = new int[newWidth * newHeight];
        bitmap.getPixels(intBuffer, 0, oldWidth, 0, 0, oldWidth, oldHeight);
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                final int tmp = intBuffer[i * oldWidth + j];
                byteBuffer.putFloat(((tmp >> 16) & 0xFF) / 255.0f);
                byteBuffer.putFloat(((tmp >> 8) & 0xFF) / 255.0f);
                byteBuffer.putFloat((tmp & 0xFF) / 255.0f);
            }
        }
        return byteBuffer;
    }

    /**
     * 从 assets目录读取整个文件，返回 ByteBuffer
     *
     * @param assetManager getAssets() 方法
     * @param filename     文件名，相对于 assets 目录的路径
     * @return ByteBuffer 文件内容，读取失败返回 null
     */
    static public ByteBuffer readByteBufferFromAssets(
            AssetManager assetManager, final String filename) {
        AssetFileDescriptor fileDescriptor;
        try {
            fileDescriptor = assetManager.openFd(filename);
        } catch (IOException e) {
            return null;
        }
        FileInputStream inputStream = new FileInputStream(fileDescriptor.getFileDescriptor());
        FileChannel fileChannel = inputStream.getChannel();
        long startOffset = fileDescriptor.getStartOffset();
        long declaredLength = fileDescriptor.getDeclaredLength();
        ByteBuffer result;
        try {
            result = fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength);
        } catch (IOException e) {
            result = null;
        }
        return result;
    }

    static public Bitmap readBitmapFromAssets(
            AssetManager assetManager, final String filename) {
        Bitmap image;
        try {
            InputStream is = assetManager.open(filename);
            image = BitmapFactory.decodeStream(is);
            is.close();
        } catch (IOException e) {
            image = null;
            e.printStackTrace();
        }
        return image;
    }

}
