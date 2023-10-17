package com.m2.smartlock.application;

import android.app.Application;
import android.util.Log;

import io.reactivex.rxjava3.plugins.RxJavaPlugins;

public class MyApplication extends Application {

    private static final String TAG = MyApplication.class.getSimpleName();

    @Override
    public void onCreate() {
        super.onCreate();
        RxJavaPlugins.setErrorHandler(throwable -> {
            Log.d(TAG, "onErrorHandler -> " + throwable.getMessage());
        });
    }
}
