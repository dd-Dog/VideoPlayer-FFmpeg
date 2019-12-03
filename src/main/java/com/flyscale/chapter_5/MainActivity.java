package com.flyscale.chapter_5;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("NativeVideoPlayer");
    }
    private NativeVideoPlayer mNativeVideoPlayer;

    private static final String TAG = "MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mNativeVideoPlayer = new NativeVideoPlayer();
        String stringFromJNI = mNativeVideoPlayer.getStringFromJNI("Hello, I am Java!");
        Log.d(TAG, "stringFromJNI=" + stringFromJNI);

        findViewById(R.id.test).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.test:
                mNativeVideoPlayer.test();
                break;
        }
    }
}
