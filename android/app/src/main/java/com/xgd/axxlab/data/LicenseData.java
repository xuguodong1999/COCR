package com.xgd.axxlab.data;

public class LicenseData {
    String name;

    public String getName() {
        return name;
    }

    public String getDetail() {
        return detail;
    }

    public int getImageId() {
        return imageId;
    }

    String detail;
    int imageId;

    public LicenseData(String name, String detail, int imageId) {
        this.name = name;
        this.detail = detail;
        this.imageId = imageId;
    }
}
