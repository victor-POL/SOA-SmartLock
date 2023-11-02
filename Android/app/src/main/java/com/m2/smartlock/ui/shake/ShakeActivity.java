package com.m2.smartlock.ui.shake;

import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;
import com.m2.smartlock.remote.AppMqttConstants;
import com.m2.smartlock.remote.MqttPublisher;

import org.eclipse.paho.client.mqttv3.MqttException;

import io.reactivex.rxjava3.disposables.Disposable;

public class ShakeActivity extends AppCompatActivity implements SensorEventListener {

    private static final String TAG = ShakeActivity.class.getSimpleName();
    private MqttPublisher publisher;
    private Disposable disposablePublish;
    private SensorManager sensorManager;
    private boolean ignoreShake = false; // disable publisher when popup is showing

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_shake);
        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setNavigationOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

        try {
            publisher = new MqttPublisher();
        } catch (MqttException e) {
            e.printStackTrace();
            Toast.makeText(this, R.string.publisher_error, Toast.LENGTH_SHORT).show();
            finish();
        }
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sensorManager != null) {
            Sensor accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            if (accelerometer != null) {
                sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    }

    private void onShakeDetected() {
        Log.i(TAG, "onShakeDetected");
        if (ignoreShake)
            return;

        Toast.makeText(this, "Shake detectado", Toast.LENGTH_SHORT).show();

        if (publisher == null) {
            Log.e(TAG, "publisher could not started.");
            return;
        }

        if (disposablePublish != null && !disposablePublish.isDisposed())
            disposablePublish.dispose();
        disposablePublish = publisher
                .publish(AppMqttConstants.TOPIC_APP_COMMAND, AppMqttConstants.TOPIC_VALUE_APP_COMMAND_UNLOCK)
                .subscribe(
                        this::unlockSuccess,
                        throwable -> unlockError()
                );
    }

    private void unlockSuccess(String updatedPassword) {
        Intent intent = new Intent(this, ShakeSuccessActivity.class);
        startActivity(intent);
    }

    private void unlockError() {
        ignoreShake = true;
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle(R.string.app_name)
                .setCancelable(false)
                .setMessage(R.string.unlock_error)
                .setPositiveButton(R.string.accept, (dialog, which) -> {
                    ignoreShake = false;
                    dialog.dismiss();
                });
        builder.create().show();
    }

    @Override
    protected void onDestroy() {
        if (publisher != null)
            publisher.disconnect();
        if (disposablePublish != null && !disposablePublish.isDisposed())
            disposablePublish.dispose();
        super.onDestroy();
    }

    public void onSensorChanged(SensorEvent event) {
        float x = event.values[0];
        float y = event.values[1];
        float z = event.values[2];

        double acceleration = Math.sqrt(x * x + y * y + z * z);
        if (acceleration > 15) {
            onShakeDetected();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Do something if sensor accuracy changes
    }

    @Override
    protected void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (sensorManager != null) {
            Sensor accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            if (accelerometer != null) {
                sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    }
}
