package com.xgd.axxlab;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class MainActivity2 extends BaseActivity {
    private static final String TAG = "AxxxMainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate: ");
        setContentView(R.layout.activity_main2);
//        ActionBar actionBar=getSupportActionBar();
//        if(actionBar!=null){
//            actionBar.hide();
//        }


        Button button1 = findViewById(R.id.btn1);
        button1.setText("run model");
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                helloWorld();
            }
        });
        Button button2 = findViewById(R.id.btn2);
        button2.setText("detete model");
        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                goodbyeWorld();
            }
        });
        Button button3 = findViewById(R.id.btn3);
        button3.setText("open scribble activity");
        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent("com.xgd.axxlab.OPEN_SCRIBBLE");
                intent.addCategory("com.xgd.axxlab.CATEGORY_OPEN_SCRIBBLE");
                startActivityForResult(intent, 1);
            }
        });
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        String toSaveData = "i save these dara";
        outState.putString("save_data_key", toSaveData);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case 1:
                String s = data.getStringExtra("data_on_scribble_finish");
                Log.d(TAG, "onActivityResult: " + s);
                break;
            default:
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(TAG, "onStart: ");
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart: ");
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume: ");
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(TAG, "onPause: ");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop: ");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy: ");
    }

    @Override
    public void onBackPressed() {
        long currentTimeMillis = System.currentTimeMillis();
        if (currentTimeMillis - lastTryExitTimestamp > R.integer.press_to_exit_interval_ms) {
            lastTryExitTimestamp = currentTimeMillis;
            Toast.makeText(getBaseContext(),
                    R.string.press_again_to_exit, Toast.LENGTH_SHORT).show();
        } else {
            super.onBackPressed();
        }
    }

    long lastTryExitTimestamp = -1;


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_add_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem menuItem) {
        Intent intent = null;
        switch (menuItem.getItemId()) {
            case R.id.main_add_website: {
                intent = new Intent(Intent.ACTION_VIEW);
                intent.setData(Uri.parse(getResources().getString(R.string.website_link)));
                break;
            }
            case R.id.main_add_media: {
                intent = new Intent("com.xgd.axxlab.OPEN_MEDIA");
                intent.addCategory("com.xgd.axxlab.CATEGORY_OPEN_MEDIA");
                break;
            }
            case R.id.main_add_phone: {
                intent = new Intent(Intent.ACTION_DIAL);
                intent.setData(Uri.parse(getResources().getString(R.string.phone_link)));
                break;
            }
            case R.id.main_add_scribble: {
                intent = new Intent(MainActivity2.this, ScribbleActivity.class);
                intent.putExtra("data_from_main_activity", "main say hello to scribble");
                break;
            }
            default:
                break;
        }
        if (intent != null) {
            startActivity(intent);
        }
        return true;
    }
}
