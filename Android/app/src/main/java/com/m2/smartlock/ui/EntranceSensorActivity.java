package com.m2.smartlock.ui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.color.MaterialColors;
import com.m2.smartlock.R;
import com.m2.smartlock.remote.AppMqttConstants;
import com.m2.smartlock.remote.MqttHandler;

public class EntranceSensorActivity extends AppCompatActivity {

    private static final int MIN_VALUE_FOR_DETECT_PERSON = 30;
    private MqttHandler mqttHandler;
    private final EntranceSensorValueReceiver receiver = new EntranceSensorValueReceiver();

    private ImageView ivSensor;
    private TextView tvSensorValue;

    private int colorSensorFar, colorSensorClose;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_entrance_sensor);
        ivSensor = findViewById(R.id.ivSensor);
        tvSensorValue = findViewById(R.id.tvSensorValue);

        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setNavigationOnClickListener(v -> {
            getOnBackPressedDispatcher().onBackPressed();
        });

        colorSensorFar = MaterialColors.getColor(this, R.attr.colorSensorFar, Color.BLACK);
        colorSensorClose = MaterialColors.getColor(this, R.attr.colorSensorClose, Color.WHITE);

        updateValue(null);

        mqttHandler = new MqttHandler(getApplicationContext(), EntranceSensorActivity.class.getSimpleName());
        mqttHandler.connect(AppMqttConstants.BROKER_URL, AppMqttConstants.CLIENT_ID, AppMqttConstants.USER, AppMqttConstants.PASS);
        publishMessage(AppMqttConstants.TOPIC_APP_COMMAND, "desde mobile");
        setupBroadcastReceiver();
        subscribeToTopic(AppMqttConstants.TOPIC_APP_COMMAND);
    }

    private void setupBroadcastReceiver() {
        //se asocia(registra) la  accion RESPUESTA_OPERACION, para que cuando el Servicio de recepcion la ejecute
        //se invoque automaticamente el OnRecive del objeto receiver
        IntentFilter intentFilter = new IntentFilter(mqttHandler.getAction());
        intentFilter.addCategory(Intent.CATEGORY_DEFAULT);
        registerReceiver(receiver, intentFilter);
    }

    private void publishMessage(String topic, String message) {
        Toast.makeText(this, "Publishing message: " + message, Toast.LENGTH_SHORT).show();
        mqttHandler.publish(topic, message);
    }

    private void subscribeToTopic(String topic) {
        Toast.makeText(this, "Subscribing to topic " + topic, Toast.LENGTH_SHORT).show();
        mqttHandler.subscribe(topic);
    }

    private void updateValue(Integer value) {
        if (value == null) {
            tvSensorValue.setText(R.string.format_d_cm_default);
            return;
        }
        tvSensorValue.setText(getString(R.string.format_d_cm, value));
        ivSensor.setColorFilter(value <= MIN_VALUE_FOR_DETECT_PERSON ? colorSensorClose : colorSensorFar);
    }

    @Override
    protected void onDestroy() {
        mqttHandler.disconnect();
        super.onDestroy();
        unregisterReceiver(receiver);
    }

    public class EntranceSensorValueReceiver extends BroadcastReceiver {
        public void onReceive(Context context, Intent intent) {
            String rawMessage = intent.getStringExtra(MqttHandler.EXTRA_RAW_VALUE);
            //Float valorPote = intent.getFloatExtra("valorPote", 0);

            Integer sensorValue;
            try {
                sensorValue = Integer.parseInt(rawMessage);
            } catch (NumberFormatException e) {
                e.printStackTrace();
                sensorValue = null;
            }
            updateValue(sensorValue);

            // para probar texto, pero en teoria debe llegar numeros enteros
            // si no es numero, no formatea e imprime en crudo:
            if (sensorValue == null){
                tvSensorValue.setText(rawMessage);
            }
        }
    }

}
