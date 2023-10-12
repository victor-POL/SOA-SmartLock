package com.m2.smartlock.ui;

import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;

public class EntranceSensorActivity extends AppCompatActivity {

    private static final int MIN_VALUE_FOR_DETECT_PERSON = 30;

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
    }

    private void updateValue(int value){
        tvSensorValue.setText(getString(R.string.format_d_cm, value));
        //ivSensor.setColorFilter(ContextCompat.getColor());
    }
}
