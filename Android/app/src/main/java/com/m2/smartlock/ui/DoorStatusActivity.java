package com.m2.smartlock.ui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.core.content.ContextCompat;

import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.color.MaterialColors;
import com.m2.smartlock.R;
import com.m2.smartlock.service.AppService;

public class DoorStatusActivity extends BaseActivity {

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

        showLastDoorStatus();
        setupBroadcastReceiver();
        addBroadcastReceiverForConnectionLost();
    }

    private void showLastDoorStatus() {
        Boolean lastDoorStatus = AppService.getLastDoorStatus();
        if (lastDoorStatus == null)
            tvDoorStatus.setText(R.string.door_status_init_description);
        else
            updateValue(lastDoorStatus);
    }

    private void setupBroadcastReceiver() {
        IntentFilter intentFilter = new IntentFilter(AppService.ACTION_DOOR_STATUS);
        intentFilter.addCategory(Intent.CATEGORY_DEFAULT);
        ContextCompat.registerReceiver(this, receiver, intentFilter,  ContextCompat.RECEIVER_NOT_EXPORTED);
    }

    private void updateValue(boolean open) {
        tvDoorStatus.setText(open ? R.string.door_status_open_description : R.string.door_status_closed_description);
        ivDoorStatus.setImageResource(open ? R.drawable.ic_unlock : R.drawable.ic_lock);
        ivDoorStatus.setColorFilter(open ? colorStatusOn : colorStatusOff);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(receiver);
    }

    public class DoorStatusReceiver extends BroadcastReceiver {
        public void onReceive(Context context, Intent intent) {
            updateValue(intent.getBooleanExtra(AppService.EXTRA_OF_ACTION, false));
        }
    }

}
