package service;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class NativeService extends Service {

    private static final String TAG = "NativeService";

    @Override
    public IBinder onBind(Intent arg0) {
        // TODO Auto-generated method stub
        Log.i(TAG, "Service on bind");// 服务被绑定
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Service on create");// 服务被创建
    }
}
