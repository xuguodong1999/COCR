package com.xgd.axxlab;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

public class DialogActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dialog);
    }
    public static void actionStart(Context context,String title,String info){
        Intent intent=new Intent(context,DialogActivity.class);
        intent.putExtra("title",title);
        intent.putExtra("info",info);
        context.startActivity(intent);
    }
}