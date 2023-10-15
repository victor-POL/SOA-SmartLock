package com.m2.smartlock.service;

import android.app.Notification;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import com.m2.smartlock.R;
import com.m2.smartlock.utils.AppNotificationUtils;

import java.util.Objects;

public class AppService extends Service {

    private static final String TAG = AppService.class.getSimpleName();
    private static final String ACTION_STOP_SERVICE = "m2.smartlock.action.stop_service";
    private static final int NOTIFICATION_ID = 1;

    public static void start(Context context) {
        Intent serviceIntent = new Intent(context, AppService.class);
        context.startForegroundService(serviceIntent);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null; // No external apps
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");

        if (Objects.equals(intent.getAction(), ACTION_STOP_SERVICE)) {
            Log.d(TAG, "disconnect service request");
            AppNotificationUtils.closeNotification(getApplicationContext(), NOTIFICATION_ID);
            stopSelf();
            return super.onStartCommand(intent, flags, startId);
        }

        Notification notification = AppNotificationUtils.createServiceNotification(
                getApplicationContext(),
                getString(R.string.service_notification_title),
                getString(R.string.service_notification_body),
                AppService.class,
                getString(R.string.service_notification_cancel),
                ACTION_STOP_SERVICE);
        startForeground(NOTIFICATION_ID, notification);

        //todo subscribirse
        new Thread(() -> {
            try {
                Thread.sleep(2000);
                showNotification();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();

        return Service.START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");
    }

    private void showNotification(){
        AppNotificationUtils.showNotification(
                this,
                getString(R.string.notify_opened_door_title),
                getString(R.string.notify_opened_door_description)
        );
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
    }
}
