package com.m2.smartlock.ui.shake;

import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;
import com.m2.smartlock.service.AppService;
import com.m2.smartlock.ui.BaseActivity;


public class ShakeActivity extends BaseActivity implements SensorEventListener {

    private static final String TAG = ShakeActivity.class.getSimpleName();
    private SensorManager sensorManager;
    private ProgressBar progressBar;
    private AlertDialog alertDialogError;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_shake);
        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        progressBar = findViewById(R.id.progressBar);
        toolbar.setNavigationOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sensorManager != null) {
            Sensor accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            if (accelerometer != null) {
                sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
        addBroadcastReceiverForConnectionLost();
        addBroadcastReceiverForPublish();
    }

    private void onShakeDetected() {
        Log.i(TAG, "onShakeDetected");
        if (progressBar.getVisibility() == View.VISIBLE)
            return;

        showLoading();
        Intent intent = new Intent(this, AppService.class);
        intent.setAction(AppService.ACTION_COMMAND);
        startService(intent);
    }

    private void showLoading(){
        progressBar.setVisibility(View.VISIBLE);
    }

    private void hideLoading(){
        progressBar.setVisibility(View.GONE);
    }

    @Override
    public void onPublishSuccess(String topic) {
        if (topic.equals(AppService.ACTION_COMMAND))
            unlockSuccess();
    }

    private void unlockSuccess() {
        hideLoading();
        Intent intent = new Intent(this, ShakeSuccessActivity.class);
        startActivity(intent);
    }

    @Override
    public void onPublishError() {
        unlockError();
    }

    private void unlockError() {
        hideLoading();
        if (alertDialogError == null){
           AlertDialog.Builder builder = new AlertDialog.Builder(this)
                    .setTitle(R.string.app_name)
                    .setCancelable(false)
                    .setMessage(R.string.unlock_error)
                    .setPositiveButton(R.string.accept, (dialog, which) -> {
                        dialog.dismiss();
                    });
            alertDialogError = builder.create();
        }
        if (!alertDialogError.isShowing())
            alertDialogError.show();
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
