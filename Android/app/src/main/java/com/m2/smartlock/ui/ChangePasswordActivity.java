package com.m2.smartlock.ui;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
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

public class ChangePasswordActivity extends AppCompatActivity {

    private static final String TAG = ChangePasswordActivity.class.getSimpleName();
    private EditText etPassword;
    private Button btnSave;

    private MqttPublisher publisher;
    private Disposable disposablePublish;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_change_password);
        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setNavigationOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

        etPassword = findViewById(R.id.etPassword);
        btnSave = findViewById(R.id.btnSave);

        setupInputPassword();
        btnSave.setOnClickListener(v -> onClickSave());

        try {
            publisher = new MqttPublisher();
        } catch (MqttException e) {
            e.printStackTrace();
            Toast.makeText(this, R.string.publisher_error, Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    private void setupInputPassword() {
        TextWatcher textWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                btnSave.setEnabled(!TextUtils.isEmpty(s));
            }
        };
        etPassword.addTextChangedListener(textWatcher);
    }

    private void onClickSave() {
        String newPassword = etPassword.getText().toString();
        Log.d(TAG, "newPassword=" + newPassword);

        if (publisher == null) {
            Log.e(TAG, "publisher could not started.");
            return;
        }

        if (disposablePublish != null)
            disposablePublish.dispose();
        disposablePublish = publisher.publish(AppMqttConstants.TOPIC_APP_SET_PASS, newPassword)
                .subscribeOn(Schedulers.newThread())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onSavePasswordSuccess,
                        throwable -> onSavePasswordError()
                );
    }

    private void onSavePasswordSuccess(String updatedPassword) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle(R.string.app_name)
                .setMessage(R.string.save_password_success)
                .setPositiveButton(R.string.accept, (dialog, which) -> {
                    finish();
                });
        builder.create().show();
    }

    private void onSavePasswordError() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
                .setTitle(R.string.app_name)
                .setMessage(R.string.save_password_error)
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
