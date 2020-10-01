package com.xgd.axxlab.detector;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Build;
import android.util.Log;

import org.tensorflow.lite.Interpreter;
import org.tensorflow.lite.nnapi.NnApiDelegate;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

import static com.xgd.axxlab.detector.Config.ANCHOR_NUM;
import static com.xgd.axxlab.detector.Config.CLASS_NUM;
import static com.xgd.axxlab.detector.Config.DOWN_RATIO;
import static com.xgd.axxlab.detector.Config.HEAD_NUM;
import static com.xgd.axxlab.detector.Config.INPUT_HEIGHT;
import static com.xgd.axxlab.detector.Config.INPUT_WIDTH;
import static com.xgd.axxlab.detector.Config.LOC_PARM_NUM;
import static com.xgd.axxlab.detector.Config.ModelFileName;


public class DetectorTFLiteImpl implements Detector {
    private static final String TAG = "AxxxDetectorTFLiteImpl";
    Interpreter interpreter = null;
    int threadNum = 2;
    // FIXME: API>27，启用后崩溃，提示代理失败，不支持动态尺寸
    /*
    java.lang.IllegalArgumentException:
    Internal error:
    Failed to apply delegate:
    Attempting to use a delegate that only supports
    static-sized tensors with a graph that has dynamic-sized tensors.
     */
    boolean isNNAPIEnabled = false;
    float iouThresh = 0.5f, confThresh = 0.5f;
    NnApiDelegate nnApiDelegate = null;

    private DetectorTFLiteImpl() {

    }

    public static DetectorTFLiteImpl getInstance() {
        return Holder.instance;
    }

    @Override
    public boolean initialize(AssetManager assetManager) {
        ByteBuffer weights = Utils.readByteBufferFromAssets(assetManager, ModelFileName);
        if (weights == null) {
            return false;
        }
        Interpreter.Options options = new Interpreter.Options();
        options.setNumThreads(getThreadNum());
        if (isNNAPIEnabled) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                nnApiDelegate = new NnApiDelegate();
                options.addDelegate(nnApiDelegate);
            }
        }
        close();
        try {
            interpreter = new Interpreter(weights, options);
        } catch (Exception e) {
            close();
            options = new Interpreter.Options();
            options.setNumThreads(getThreadNum());
            interpreter = new Interpreter(weights, options);
        }
        return true;
    }

    @Override
    public int getThreadNum() {
        return threadNum;
    }

    @Override
    public void setThreadNum(int threadNum) {
        this.threadNum = threadNum;
    }

    @Override
    public void close() {
        if (interpreter != null) {
            synchronized (interpreter) {
                if (interpreter != null) {
                    interpreter.close();
                    interpreter = null;
                }
                if (nnApiDelegate != null) {
                    nnApiDelegate.close();
                    nnApiDelegate = null;
                }
            }
        }
    }

    @Override
    public boolean IsOptimizeEnabled() {
        return isNNAPIEnabled;
    }

    @Override
    public void enableOptimize(boolean isNNAPIEnabled) {
        this.isNNAPIEnabled = isNNAPIEnabled;
    }

    @Override
    public float getIOUThresh() {
        return iouThresh;
    }

    @Override
    public void setIOUThresh(float thresh) {
        iouThresh = thresh;
    }

    @Override
    public float getConfThresh() {
        return confThresh;
    }

    @Override
    public void setConfThresh(float thresh) {
        confThresh = thresh;
    }

    public native String passArrToJNI(
            float[][][][][] arr, int headNum, int anchorNum, int classNum, int locParmNum,
            int inputWidth, int inputHeight, int downRatio);

    /**
     * 输入的位图尺寸必须满足 Config 中的 INPUT_WIDTH x INPUT_HEIGHT，现在 tflite 不支持动态尺寸
     *
     * @param bitmap
     * @return
     */
    @Override
    public List<Recognition> getRecognition(Bitmap bitmap) {
        ByteBuffer input = Utils.convertBitmapToByteBuffer(bitmap);

        Map<Integer, Object> outputMap = new HashMap<>();
        int gridNumX = INPUT_WIDTH / DOWN_RATIO;
        int gridNumY = INPUT_HEIGHT / DOWN_RATIO;
        int parmNum = ANCHOR_NUM * (CLASS_NUM + LOC_PARM_NUM);
        for (int i = 0; i < HEAD_NUM; i++) {
            outputMap.put(i, new float[1][gridNumX][gridNumY][parmNum]);
            gridNumX *= 2;
            gridNumY *= 2;
        }
        long startTimestamp = System.currentTimeMillis();
        interpreter.runForMultipleInputsOutputs(new Object[]{input}, outputMap);
        long endTimestamp = System.currentTimeMillis();
        Log.d(TAG, "getRecognition: takes " + (endTimestamp - startTimestamp) + " ms");
        float[][][][][] dat = new float[HEAD_NUM][][][][];
        gridNumX = INPUT_WIDTH / DOWN_RATIO;
        gridNumY = INPUT_HEIGHT / DOWN_RATIO;
        for (int i = 0; i < HEAD_NUM; i++) {
            dat[i] = (float[][][][]) outputMap.get(i);
            Log.d(TAG, "getRecognition: d2=" + dat[i].length);
            Log.d(TAG, "getRecognition: d3=" + dat[i][0].length);
            Log.d(TAG, "getRecognition: d4=" + dat[i][0][0].length);
            Log.d(TAG, "getRecognition: d5=" + dat[i][0][0][0].length);
            for (int x = 0; x < gridNumX; x++) {
                for (int y = 0; y < gridNumY; y++) {
                    Log.d(TAG, Arrays.toString(dat[i][0][x][y]));
                }
            }
            gridNumX *= 2;
            gridNumY *= 2;
        }
//        passArrToJNI(dat, HEAD_NUM, ANCHOR_NUM, CLASS_NUM, LOC_PARM_NUM,
//                INPUT_WIDTH, INPUT_HEIGHT, DOWN_RATIO);
        return new ArrayList<>();
    }

    @Override
    public List<Recognition> getRecognition(List<List<PointF>> script) {
        Bitmap bitmap = Utils.convertScriptToBitmap(
                Utils.normalizeScript(script));
        return getRecognition(bitmap);
    }

    public List<Recognition> nms(List<Recognition> inputs) {
        ArrayList<Recognition> nmsList = new ArrayList<>();

        for (int k = 0; k < CLASS_NUM; k++) {
            //1.find max confidence per class
            PriorityQueue<Recognition> pq = new PriorityQueue<Recognition>(
                    200,
                    new Comparator<Recognition>() {
                        @Override
                        public int compare(final Recognition lhs, final Recognition rhs) {
                            // Intentionally reversed to put high confidence at the head of the queue.
                            return Float.compare(rhs.getConfidence(), lhs.getConfidence());
                        }
                    });

            for (int i = 0; i < inputs.size(); ++i) {
                if (inputs.get(i).getLabelIdx() == k) {
                    pq.add(inputs.get(i));
                }
            }

            //2.do non maximum suppression
            while (pq.size() > 0) {
                //insert detection with max confidence
                Recognition[] a = new Recognition[pq.size()];
                Recognition[] detections = pq.toArray(a);
                Recognition max = detections[0];
                nmsList.add(max);
                pq.clear();

                for (int j = 1; j < detections.length; j++) {
                    Recognition detection = detections[j];
                    RectF b = detection.getRectF();
                    if (iou(max.getRectF(), b) < getIOUThresh()) {
                        pq.add(detection);
                    }
                }
            }
        }
        return nmsList;
    }

    public float iou(RectF a, RectF b) {
        return box_intersection(a, b) / box_union(a, b);
    }

    protected float box_intersection(RectF a, RectF b) {
        float w = overlap((a.left + a.right) / 2, a.right - a.left,
                (b.left + b.right) / 2, b.right - b.left);
        float h = overlap((a.top + a.bottom) / 2, a.bottom - a.top,
                (b.top + b.bottom) / 2, b.bottom - b.top);
        if (w < 0 || h < 0) return 0;
        float area = w * h;
        return area;
    }

    protected float overlap(float x1, float w1, float x2, float w2) {
        float l1 = x1 - w1 / 2;
        float l2 = x2 - w2 / 2;
        float left = l1 > l2 ? l1 : l2;
        float r1 = x1 + w1 / 2;
        float r2 = x2 + w2 / 2;
        float right = r1 < r2 ? r1 : r2;
        return right - left;
    }

    protected float box_union(RectF a, RectF b) {
        float i = box_intersection(a, b);
        float u = (a.right - a.left) * (a.bottom - a.top) + (b.right - b.left) * (b.bottom - b.top) - i;
        return u;
    }

    static class Holder {
        private static DetectorTFLiteImpl instance = new DetectorTFLiteImpl();
    }

}
