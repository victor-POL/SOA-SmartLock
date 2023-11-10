package com.m2.smartlock.ui;

import androidx.annotation.NonNull;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.m2.smartlock.R;
import com.m2.smartlock.service.AppService;
import com.m2.smartlock.ui.shake.ShakeActivity;
import com.m2.smartlock.utils.AppNotificationUtils;

public class MainActivity extends BaseActivity
{
  private Button btnTurnOnNotifications;
  private TextView tvNotificationDescription;
  private boolean shouldCheckChannel = false;

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    setupClickableCards();
    setupNotifications();
    addBroadcastReceiverForConnectionLost();
  }

  @Override
  protected void onResume()
  {
    super.onResume();
    if (shouldCheckChannel)
    {
      shouldCheckChannel = false;
      if (AppNotificationUtils.checkAllowedChannelNotification(this))
      {
        setupAllowedNotification();
      }
      else
      {
        setupNotAllowedChannelNotification();
      }
    }
  }

  private void setupClickableCards()
  {
    View partialEntranceValueCard = findViewById(R.id.partialDoorStatusCard);
    View partialShakeCard = findViewById(R.id.partialShakeCard);
    View partialChangePasswordCard = findViewById(R.id.partialChangePasswordCard);

    partialEntranceValueCard.setOnClickListener(v -> onClickEntranceSensor());
    partialShakeCard.setOnClickListener(v -> onClickShake());
    partialChangePasswordCard.setOnClickListener(v -> onClickChangePassword());
  }

  private void setupNotifications()
  {
    btnTurnOnNotifications = findViewById(R.id.btnTurnOnNotifications);
    tvNotificationDescription = findViewById(R.id.partialNotificationCard).findViewById(R.id.tvDescription);

    if (AppNotificationUtils.checkAllowedNotification(this))
    {
      if (AppNotificationUtils.checkAllowedChannelNotification(this))
      {
        setupAllowedNotification();
      }
      else
      {
        setupNotAllowedChannelNotification();
      }
    }
    else
    {
      setupNotAllowedNotification();
    }
  }

  private void setupNotAllowedNotification()
  {
    tvNotificationDescription.setText(R.string.notifications_description_warning);
    btnTurnOnNotifications.setOnClickListener(v -> AppNotificationUtils.requestPermission(this));
    btnTurnOnNotifications.setVisibility(View.VISIBLE);
    AppService.startAsBackground(this);
  }

  private void setupNotAllowedChannelNotification()
  {
    tvNotificationDescription.setText(R.string.notifications_description_warning_channel);
    btnTurnOnNotifications.setOnClickListener(v ->
    {
      shouldCheckChannel = true;
      AppNotificationUtils.launchChannelSettings(this);
    });
    btnTurnOnNotifications.setVisibility(View.VISIBLE);
    AppService.startAsBackground(this);
  }

  private void setupAllowedNotification()
  {
    tvNotificationDescription.setText(R.string.notifications_description);
    btnTurnOnNotifications.setOnClickListener(null);
    btnTurnOnNotifications.setVisibility(View.GONE);
    AppService.startAsForeground(this);
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults)
  {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    if (AppNotificationUtils.onRequestPermissionResult(requestCode, permissions, grantResults))
    {
      setupAllowedNotification();
    }
  }

  private void onClickEntranceSensor()
  {
    startActivity(new Intent(this, DoorStatusActivity.class));
  }

  private void onClickShake()
  {
    startActivity(new Intent(this, ShakeActivity.class));
  }

  private void onClickChangePassword()
  {
    startActivity(new Intent(this, ChangePasswordActivity.class));
  }
}