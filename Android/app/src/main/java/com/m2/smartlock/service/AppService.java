package com.m2.smartlock.service;

import android.app.Notification;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import com.m2.smartlock.R;
import com.m2.smartlock.remote.AppMqttConstants;
import com.m2.smartlock.utils.AppNotificationUtils;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.util.Objects;

public class AppService extends Service {

    private static final String TAG = AppService.class.getSimpleName();
    private static @Nullable Boolean lastDoorStatus;
    private static @Nullable Boolean clientConnected;
    private static final String EXTRA_FOREGROUND_ENABLED = "foreground_enabled";

    // Actions that service send to activities
    public static final String ACTION_DOOR_STATUS = "m2.smartlock.action.door_status";
    public static final String ACTION_CONNECTION_LOST = "m2.smartlock.action.connection_lost";
    public static final String ACTION_PUBLISHED = "m2.smartlock.action.published";

    // Actions that service receive from activities
    public static final String ACTION_STOP_FOREGROUND_SERVICE = "m2.smartlock.action.stop_f_service";
    public static final String ACTION_SET_PASS = "m2.smartlock.action.set_pass";
    public static final String ACTION_COMMAND = "m2.smartlock.action.command";
    public static final String EXTRA_OF_ACTION = "extra";

    private Thread threadPublish;
    private static final int NOTIFICATION_ID = 1;
    private MqttClient client;

    public static void startAsBackground(Context context) {
        Intent serviceIntent = new Intent(context, AppService.class);
        context.startService(serviceIntent);
    }

    // Required notification permissions
    public static void startAsForeground(Context context) {
        Intent serviceIntent = new Intent(context, AppService.class);
        serviceIntent.putExtra(EXTRA_FOREGROUND_ENABLED, true);
        context.startForegroundService(serviceIntent);
    }

    @Nullable
    public static Boolean getLastDoorStatus(){
        return lastDoorStatus;
    }

    @Nullable
    public static Boolean getClientConnected(){
        return clientConnected;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null; // No external apps
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");

        if (handleActionRequest(intent.getAction(), intent.getStringExtra(EXTRA_OF_ACTION))){
            return super.onStartCommand(intent, flags, startId);
        }

        setup(intent.getBooleanExtra(EXTRA_FOREGROUND_ENABLED, false));
        return Service.START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");
    }

    private void setup(boolean foregroundEnabled) {
        if (foregroundEnabled) {
            Notification notification = AppNotificationUtils.createServiceNotification(
                    getApplicationContext(),
                    getString(R.string.service_notification_title),
                    getString(R.string.service_notification_body),
                    AppService.class,
                    getString(R.string.service_notification_cancel),
                    ACTION_STOP_FOREGROUND_SERVICE);
            startForeground(NOTIFICATION_ID, notification);
        }

        if (clientConnected == null || !clientConnected) {
            connect();
            subscribe();
        }
    }

    private void connect() {
        try {
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setUserName(AppMqttConstants.USER);
            options.setPassword(AppMqttConstants.PASS.toCharArray());

            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            client = new MqttClient(AppMqttConstants.BROKER_URL, AppMqttConstants.CLIENT_ID, persistence);
            client.connect(options);
            client.setCallback(createMqttCallback());
            client.setTimeToWait(10_000);
            clientConnected = true;
        } catch (MqttException e) {
            clientConnected = false;
            Log.e(TAG, "connect() failed");
            e.printStackTrace();
        }
    }

    private void subscribe() {
        String[] topics = {AppMqttConstants.TOPIC_ESP_DOOR_STATUS, AppMqttConstants.TOPIC_ESP_NOTIFY};
        try {
            client.subscribe(topics);
        } catch (MqttException e) {
            Log.e(TAG, "subscribe() failed");
            e.printStackTrace();
        }
    }

    private MqttCallback createMqttCallback(){
        return new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                Log.e(TAG, "connectionLost", cause);
                sendAction(ACTION_CONNECTION_LOST);
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) {
                Log.i(TAG, "messageArrived");
                if (Objects.equals(topic, AppMqttConstants.TOPIC_ESP_NOTIFY) &&
                        Objects.equals(topic, AppMqttConstants.TOPIC_VALUE_ESP_NOTIFY_DOOR_OPEN_TIMEOUT)) {
                    showNotification();
                } else if (Objects.equals(topic, AppMqttConstants.TOPIC_ESP_DOOR_STATUS)) {
                    String value = message.toString();
                    if (Objects.equals(value, AppMqttConstants.TOPIC_VALUE_ESP_DOOR_STATUS_OPEN)){
                        lastDoorStatus = true;
                        sendAction(ACTION_DOOR_STATUS, true);
                    }
                    else if (Objects.equals(value, AppMqttConstants.TOPIC_VALUE_ESP_DOOR_STATUS_CLOSED)){
                        lastDoorStatus = false;
                        sendAction(ACTION_DOOR_STATUS, false);
                    }
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.i(TAG, "deliveryComplete");
                String[] topics = token.getTopics();
                if (topics.length > 0){
                    sendAction(ACTION_PUBLISHED, getActionOfTopic(topics[0]));
                }
            }
        };
    }

    private String getActionOfTopic(String topic){
        if (topic.equals(AppMqttConstants.TOPIC_APP_SET_PASS)){
            return AppService.ACTION_SET_PASS;
        }
        if (topic.equals(AppMqttConstants.TOPIC_APP_COMMAND)){
            return AppService.ACTION_COMMAND;
        }
        if (topic.equals(AppMqttConstants.TOPIC_ESP_DOOR_STATUS)){
            return AppService.ACTION_DOOR_STATUS;
        }
        return null;
    }

    private void sendAction(String action){
        Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    private void sendAction(String action, Boolean extra){
        Intent intent = new Intent(action);
        intent.putExtra(EXTRA_OF_ACTION, extra);
        sendBroadcast(intent);
    }

    private void sendAction(String action, String extra){
        Intent intent = new Intent(action);
        intent.putExtra(EXTRA_OF_ACTION, extra);
        sendBroadcast(intent);
    }

    private void showNotification(){
        AppNotificationUtils.showNotification(
                this,
                getString(R.string.notify_opened_door_title),
                getString(R.string.notify_opened_door_description)
        );
    }

    private boolean handleActionRequest(String actionReceived,@Nullable String extra){
        if (Objects.equals(actionReceived, ACTION_STOP_FOREGROUND_SERVICE)) {
            Log.d(TAG, "disconnect service request");
            AppNotificationUtils.closeNotification(getApplicationContext(), NOTIFICATION_ID);
            stopForeground(true);
            return true;
        }

        if (Objects.equals(actionReceived, ACTION_SET_PASS) && extra != null){
            publish(AppMqttConstants.TOPIC_APP_SET_PASS, extra);
            return true;
        }

        if (Objects.equals(actionReceived, ACTION_COMMAND)){
            publish(AppMqttConstants.TOPIC_APP_COMMAND, AppMqttConstants.TOPIC_VALUE_APP_COMMAND_UNLOCK);
            return true;
        }

        return false;
    }

    private void publish(String topic, String message) {
        if (threadPublish != null && threadPublish.isAlive()){
            sendAction(ACTION_PUBLISHED);
            return;
        }

        threadPublish = new Thread(() -> {
            try {
                MqttMessage mqttMessage = new MqttMessage(message.getBytes());
                mqttMessage.setQos(2);
                client.publish(topic, mqttMessage);
            } catch (MqttException e) {
                e.printStackTrace();
            }
        });
        threadPublish.start();
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
        try {
            client.disconnect();
            clientConnected = false;
            client = null;
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
