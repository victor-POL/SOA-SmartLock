package com.m2.smartlock.ui;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;
import com.m2.smartlock.remote.AppMqttConstants;
import com.m2.smartlock.remote.MqttPublisher;

import org.eclipse.paho.client.mqttv3.MqttException;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.disposables.Disposable;
import io.reactivex.rxjava3.schedulers.Schedulers;

public class ShakeActivity extends AppCompatActivity {

    private static final String TAG = ShakeActivity.class.getSimpleName();
    private MqttPublisher publisher;
    private Disposable disposablePublish;

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

        // todo - solo para probar
        Button btnShake = findViewById(R.id.btnShake);
        btnShake.setOnClickListener(v -> onShakeDetected());
    }

    private void onShakeDetected() {
        Toast.makeText(this, "Shake detectado", Toast.LENGTH_SHORT).show();
        if (publisher == null) {
            Log.e(TAG, "publisher could not started.");
            return;
        }

        if (disposablePublish != null)
            disposablePublish.dispose();
        disposablePublish = publisher
                .publish(AppMqttConstants.TOPIC_APP_COMMAND, AppMqttConstants.TOPIC_VALUE_APP_COMMAND_UNLOCK)
                .subscribeOn(Schedulers.newThread())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::unlockSuccess,
                        throwable -> unlockError()
                );
    }

    private void unlockSuccess(String updatedPassword) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle(R.string.app_name)
                .setMessage(R.string.unlock_success)
                .setPositiveButton(R.string.accept, (dialog, which) -> {
                    finish();
                });
        builder.create().show();
    }

    private void unlockError() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle(R.string.app_name)
                .setMessage(R.string.unlock_error)
                .setPositiveButton(R.string.accept, (dialog, which) -> {
                    dialog.dismiss();
                });
        builder.create().show();
    }

    @Override
    protected void onDestroy() {
        if (publisher != null)
            publisher.disconnect();
        if (disposablePublish != null)
            disposablePublish.dispose();
        super.onDestroy();
    }
}
