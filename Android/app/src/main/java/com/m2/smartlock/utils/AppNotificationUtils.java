package com.m2.smartlock.utils;


import android.Manifest;
import android.app.Activity;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.provider.Settings;
import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.content.ContextCompat;

import com.m2.smartlock.ui.MainActivity;
import com.m2.smartlock.R;

public class AppNotificationUtils {

    private static int REQUEST_CODE_PERMISSION = 9;

    public static boolean checkAllowedNotification(Context context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            return ContextCompat.checkSelfPermission(context, Manifest.permission.POST_NOTIFICATIONS)
                    == PackageManager.PERMISSION_GRANTED;
        }
        return true;
    }

    public static boolean checkAllowedChannelNotification(Context context) {
        String channelId = context.getString(R.string.app_notification_channel_id);
        if (!TextUtils.isEmpty(channelId)) {
            NotificationManager notificationManager =
                    (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
            NotificationChannel notificationChannel = notificationManager.getNotificationChannel(channelId);
            return notificationChannel != null && notificationChannel.getImportance() != NotificationManager.IMPORTANCE_NONE;
        }
        return false;
    }

    public static void launchChannelSettings(Context context) {
        Intent intent = new Intent(Settings.ACTION_CHANNEL_NOTIFICATION_SETTINGS)
                .putExtra(Settings.EXTRA_APP_PACKAGE, context.getPackageName())
                .putExtra(Settings.EXTRA_CHANNEL_ID, context.getString(R.string.app_notification_channel_id));
        context.startActivity(intent);
    }

    public static void launchNotificationSettings(Context context){
        Intent intent = new Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS)
                .putExtra(Settings.EXTRA_APP_PACKAGE, context.getPackageName());
        context.startActivity(intent);
    }

    public static void requestPermission(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ActivityCompat.requestPermissions(
                    activity, new String[]{Manifest.permission.POST_NOTIFICATIONS}, REQUEST_CODE_PERMISSION);
        }
    }

    public static boolean onRequestPermissionResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == REQUEST_CODE_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
                return true;
        }
        return false;
    }

    public static void showNotification(Context context, String title, String body) {
        // intent to open activity when user click notification
        Intent intent = new Intent(context, MainActivity.class);

        int flags = PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT;
        PendingIntent pendingIntent =
                PendingIntent.getActivity(context, 0, intent, flags);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(
                context,
                context.getString(R.string.app_notification_channel_id)
        )
                .setSmallIcon(R.drawable.ic_app_notification)
                .setContentTitle(title)
                .setContentText(body)
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setStyle(
                        new NotificationCompat.BigTextStyle()
                                .bigText(body)
                )
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);

        NotificationManager notificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        // create channel >= API 26
        NotificationChannel channel = notificationManager.getNotificationChannel(context.getString(R.string.app_notification_channel_id));
        if (channel == null)
            channel = new NotificationChannel(
                context.getString(R.string.app_notification_channel_id),
                context.getString(R.string.app_notification_channel_title),
                NotificationManager.IMPORTANCE_HIGH
        );
        notificationManager.createNotificationChannel(channel);

        Notification notification = builder.build();
        notificationManager.notify((int) System.currentTimeMillis(), notification);
    }
}
