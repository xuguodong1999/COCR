package activity;

import org.qtproject.qt5.android.bindings.QtActivity;

import android.content.Context;
import android.content.Intent;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.app.PendingIntent;
import android.os.IBinder;
import android.os.Bundle;
import android.util.Log;
import android.graphics.Color;
import android.view.View;
import android.view.WindowManager;

import service.NativeService;

/**
 * 消除 MainActivity 的顶栏、底部状态栏
 * 参考资料1：https://blog.csdn.net/weixin_44377507/article/details/99410648
 * 参考资料2：https://blog.csdn.net/jun4331247/article/details/51182390
 */
public class MainActivity extends QtActivity {
    private static final String TAG = "MainActivity";
    private static Context context;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = getApplicationContext();

        Intent bindIntent = new Intent(MainActivity.this, NativeService.class);
        bindService(bindIntent, mCon, Context.BIND_AUTO_CREATE);
        startService(new Intent(bindIntent));
    }
    /** 当活动即将可见时调用 */
    @Override
    protected void onStart() {
        super.onStart();
        Log.d(TAG, "The onStart() event");
        getWindow().setStatusBarColor(Color.TRANSPARENT);
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_IMMERSIVE
        );
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);// 透明导航栏
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);// 透明状态栏

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                        View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_IMMERSIVE
        );
    }

    /** 当活动可见时调用 */
    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "The onResume() event");
    }

    /** 当其他活动获得焦点时调用 */
    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "The onPause() event");
    }

    /** 当活动不再可见时调用 */
    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "The onStop() event");
    }

    /** 当活动将被销毁时调用 */
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "The onDestroy() event");
    }
    private static ServiceConnection mCon = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            // TODO Auto-generated method stub
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            // TODO Auto-generated method stub
        }
    };

    public static Context getContext() {
        return context;
    }
}
