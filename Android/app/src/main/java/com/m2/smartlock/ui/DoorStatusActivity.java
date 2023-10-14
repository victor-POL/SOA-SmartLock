package com.m2.smartlock.ui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.color.MaterialColors;
import com.m2.smartlock.R;
import com.m2.smartlock.remote.AppMqttConstants;
import com.m2.smartlock.remote.MqttHandler;

public class DoorStatusActivity extends AppCompatActivity {

    private MqttHandler mqttHandler;
    private final DoorStatusReceiver receiver = new DoorStatusReceiver();

    private ImageView ivDoorStatus;
    private TextView tvDoorStatus;

    private int colorStatusOff, colorStatusOn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_door_status);
        ivDoorStatus = findViewById(R.id.ivDoorStatus);
        tvDoorStatus = findViewById(R.id.tvDoorStatus);

        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setNavigationOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

        colorStatusOff = MaterialColors.getColor(this, R.attr.colorStatusOff, Color.BLACK);
        colorStatusOn = MaterialColors.getColor(this, R.attr.colorStatusOn, Color.WHITE);

        mqttHandler = new MqttHandler(getApplicationContext(), DoorStatusActivity.class.getSimpleName());
        mqttHandler.connect();
        setupBroadcastReceiver();
        subscribeToTopic(AppMqttConstants.TOPIC_ESP_DOOR_STATUS);
    }

    private void setupBroadcastReceiver() {
        // Se asocia(registra) la accion, para que cuando el Servicio de recepcion la ejecute
        // se invoque automaticamente el OnReceive del objeto receiver
        IntentFilter intentFilter = new IntentFilter(mqttHandler.getAction());
        intentFilter.addCategory(Intent.CATEGORY_DEFAULT);
        registerReceiver(receiver, intentFilter);
    }

    private void publishMessage(String topic, String message) {
        mqttHandler.publish(topic, message);
    }

    private void subscribeToTopic(String topic) {
        mqttHandler.subscribe(topic);
        tvDoorStatus.setText(R.string.door_status_init_description);
    }

    private void updateValue(boolean open) {
        tvDoorStatus.setText(open ? R.string.door_status_open_description : R.string.door_status_closed_description);
        ivDoorStatus.setImageResource(open ? R.drawable.ic_unlock : R.drawable.ic_lock);
        ivDoorStatus.setColorFilter(open ? colorStatusOn : colorStatusOff);
    }

    @Override
    protected void onDestroy() {
        mqttHandler.disconnect();
        super.onDestroy();
        unregisterReceiver(receiver);
    }

    public class DoorStatusReceiver extends BroadcastReceiver {
        public void onReceive(Context context, Intent intent) {
            String rawMessage = intent.getStringExtra(MqttHandler.EXTRA_RAW_VALUE);
            if (rawMessage != null){
                if (rawMessage.equals(AppMqttConstants.TOPIC_VALUE_ESP_DOOR_STATUS_OPEN))
                    updateValue(true);
                else if (rawMessage.equals(AppMqttConstants.TOPIC_VALUE_ESP_DOOR_STATUS_CLOSED))
                    updateValue(false);
            }
        }
    }

}
