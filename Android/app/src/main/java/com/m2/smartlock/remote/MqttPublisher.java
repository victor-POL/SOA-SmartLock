package com.m2.smartlock.remote;

import android.util.Log;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.schedulers.Schedulers;

public class MqttPublisher {
    static final String TAG = MqttPublisher.class.getSimpleName();
    private static final int QUALITY_OF_SERVICE = 2;
    private final MqttClient client;

    public MqttPublisher() throws MqttException {
        MemoryPersistence persistence = new MemoryPersistence();
        client = new MqttClient(AppMqttConstants.BROKER_URL, AppMqttConstants.CLIENT_ID, persistence);

        MqttConnectOptions options = new MqttConnectOptions();
        options.setCleanSession(true);
        options.setUserName(AppMqttConstants.USER);
        options.setPassword(AppMqttConstants.PASS.toCharArray());
        //options.setConnectionTimeout(7);
        //client.setTimeToWait(10);
        client.connect(options);
    }

    public <T> Observable<String> publish(String topic, T data) {
        Observable<String> obs = Observable.create(emitter -> {
            try {
                String content = data.toString();

                MqttMessage message = new MqttMessage(content.getBytes());
                message.setQos(QUALITY_OF_SERVICE);
                client.publish(topic, message);

                emitter.onNext(content);
                emitter.onComplete();
            } catch (Exception e) {
                Log.e(TAG, "publishing error");
                e.printStackTrace();
                emitter.onError(e);
            }
        });
        return obs
                .subscribeOn(Schedulers.newThread())
                .doOnNext(c -> Log.d(TAG, "'" + c + "' published on topic '" + topic + "'"))
                .doOnError(throwable -> {
                    Log.e(TAG, "message not published");
                })
                .observeOn(AndroidSchedulers.mainThread());
    }

    public void disconnect() {
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
