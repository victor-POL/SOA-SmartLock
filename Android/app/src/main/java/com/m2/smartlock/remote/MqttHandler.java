package com.m2.smartlock.remote;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class MqttHandler implements MqttCallback {

    static final String TAG = MqttHandler.class.getSimpleName();
    public static final String EXTRA_RAW_VALUE = "extra_raw_value";
    private final String action;  // solo es un nombre para encontrar las actions que nos interesan dentro de la app, nada de conexiones externas
    private MqttClient client;
    private final Context mContext;

    public MqttHandler(Context mContext, String simpleAction) {
        this.mContext = mContext;
        this.action = mContext.getPackageName() + ".intent.action." + simpleAction;
    }

    public String getAction() {
        return action;
    }

    public void connect(String brokerUrl, String clientId, String username, String password) {
        try {
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setUserName(username);
            options.setPassword(password.toCharArray());

            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            client = new MqttClient(brokerUrl, clientId, persistence);
            client.connect(options);

            client.setCallback(this);
        } catch (MqttException e) {
            e.printStackTrace();
            Log.d(TAG, e.getMessage() + "  " + e.getCause());
        }
    }

    public void disconnect() {
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void publish(String topic, String message) {
        try {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            mqttMessage.setQos(2);
            client.publish(topic, mqttMessage);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void subscribe(String topic) {
        try {
            client.subscribe(topic);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        cause.printStackTrace();
        Log.d(TAG, "connectionLost " + cause.getMessage());
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String rawMessage = message.toString();
        Log.d(TAG, "messageArrived " + rawMessage);

        //String msgJson = message.toString();
        //JSONObject json = new JSONObject(message.toString());
        //Float valorPote = Float.parseFloat(json.getString("value"));

        Intent i = new Intent(action);
        i.putExtra(EXTRA_RAW_VALUE, rawMessage);
        mContext.sendBroadcast(i); // send data to broadcast observer (an activity).
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {

    }
}
