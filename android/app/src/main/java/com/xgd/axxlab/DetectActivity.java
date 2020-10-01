package com.xgd.axxlab;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.xgd.axxlab.detector.Detector;
import com.xgd.axxlab.detector.DetectorTFLiteImpl;
import com.xgd.axxlab.detector.Utils;

public class DetectActivity extends AppCompatActivity {
    private static final String TAG = "AxxxDetectActivity";

    static {
        System.loadLibrary("native-lib");
    }

    Detector detector;

    void helloWorld() {
        String s = stringFromJNI();
        Log.d(TAG, "onCreate: " + s);
        detector = DetectorTFLiteImpl.getInstance();
        detector.enableOptimize(true);
        detector.setThreadNum(4);
        detector.setConfThresh(0.5f);
        detector.setIOUThresh(0.5f);
        detector.initialize(getAssets());
        Bitmap bitmap = Utils.readBitmapFromAssets(getAssets(), "1a0.jpg");
        if (bitmap == null) {
            Log.e(TAG, "onCreate: fail to read image from assets");
        } else {
            detector.getRecognition(bitmap);
        }
    }

    void goodbyeWorld() {
        if (detector != null) {
            detector.close();
            detector = null;
        }
    }

    public native String stringFromJNI();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detect);
    }
}