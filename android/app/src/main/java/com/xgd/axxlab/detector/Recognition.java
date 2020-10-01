package com.xgd.axxlab.detector;

import android.graphics.Rect;
import android.graphics.RectF;

public class Recognition {
    int idx, x0, y0, x1, y1;
    float conf;

    public Recognition(int x0, int y0, int x1, int y1, int idx, float conf) {
        setTopLeft(x0, y0);
        setRightBottom(x1, y1);
        setLabelIdx(idx);
        this.conf = conf;
    }

    public void setRect(Rect rect) {
        x0 = rect.left;
        y0 = rect.top;
        x1 = rect.right;
        y1 = rect.bottom;
    }

    public void setRectF(RectF rect) {
        x0 = Math.max(0, Math.round(rect.left));
        y0 = Math.max(0, Math.round(rect.top));
        x1 = Math.max(0, Math.round(rect.right));
        y1 = Math.max(0, Math.round(rect.bottom));
    }

    public void setTopLeft(int x0, int y0) {
        this.x0 = x0;
        this.y0 = y0;
    }

    public RectF getRectF() {
        return new RectF(x0, y0, x1, y1);
    }

    public Rect getRect() {
        return new Rect(x0, y0, x1, y1);
    }

    public void setRightBottom(int x0, int y0) {
        this.x1 = x1;
        this.y1 = y1;
    }

    public void setLabelIdx(int idx) {
        this.idx = idx;
    }


    public int width() {
        return x1 - x0;
    }

    public int height() {
        return y1 - y0;
    }

    public int getLabelIdx() {
        return idx;
    }

    public float getConfidence() {
        return conf;
    }
}
