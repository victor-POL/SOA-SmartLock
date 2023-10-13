package com.m2.smartlock.ui;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;

public class ChangePasswordActivity extends AppCompatActivity {

    private static final String TAG = ChangePasswordActivity.class.getSimpleName();
    private EditText etPassword;
    private Button btnSave;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_change_password);
        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setNavigationOnClickListener(v -> {
            getOnBackPressedDispatcher().onBackPressed();
        });

        etPassword = findViewById(R.id.etPassword);
        btnSave = findViewById(R.id.btnSave);

        setupInputPassword();
        btnSave.setOnClickListener(v -> onClickSave());
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
        // todo send
    }

}
