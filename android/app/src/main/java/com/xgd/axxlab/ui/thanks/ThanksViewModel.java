package com.xgd.axxlab.ui.thanks;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.xgd.axxlab.R;
import com.xgd.axxlab.data.LicenseData;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class ThanksViewModel extends ViewModel {
    private static final String TAG = "AxxxThanksViewModel";
    private MutableLiveData<List<LicenseData>> mData;

    public ThanksViewModel() {
        mData = new MutableLiveData<>();
    }

    public void initData(Context context) {
        // TODO: fill in mData
        List<LicenseData> mList = new ArrayList<>();
        //Fruit orange = new Fruit(getRandomLengthName("Orange"), R.drawable.orange_pic);
        //            fruitList.add(orange);
        AssetManager assetManager = context.getResources().getAssets();
        String[] fileNames = new String[]{};
        try {
            fileNames = assetManager.list("license");
        } catch (Exception e) {
            Log.e(TAG, "initData: fail to open assets/license");
        }
        for (int i = 0; i < fileNames.length; i++) {
            Log.d(TAG, "initData: " + fileNames[i]);
            StringBuilder stringBuilder = new StringBuilder();
            try {
                BufferedReader bf = new BufferedReader(new InputStreamReader(
                        assetManager.open("license/" + fileNames[i] + "/LICENSE"),
                        "UTF-8"));
                String line;
                while (null != (line = bf.readLine())) stringBuilder.append(line);
                LicenseData data = new LicenseData(
                        fileNames[i], stringBuilder.toString(), R.drawable.opensource);
                mList.add(data);
            } catch (Exception e) {
                Log.i(TAG, "initData: fail to load " + fileNames[i]);
            }
        }
        mData.setValue(mList);
    }

    public LiveData<List<LicenseData>> getData() {
        return mData;
    }
}
