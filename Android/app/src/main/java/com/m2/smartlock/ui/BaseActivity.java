package com.m2.smartlock.ui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.m2.smartlock.R;
import com.m2.smartlock.service.AppService;

abstract public class BaseActivity extends AppCompatActivity
{
  private ConnectionLostReceiver connectionLostReceiver;
  private PublishFinishedReceiver publishFinishedReceiver;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    Boolean clientConnected = AppService.getClientConnected();
    if (clientConnected != null && !clientConnected)
    {
      onClientNotConnected();
    }
  }

  protected void addBroadcastReceiverForPublish()
  {
    if (publishFinishedReceiver != null) return;
    publishFinishedReceiver = new PublishFinishedReceiver();
    registerReceiver(AppService.ACTION_PUBLISHED, publishFinishedReceiver);
  }

  protected void addBroadcastReceiverForConnectionLost()
  {
    if (connectionLostReceiver != null) return;
    connectionLostReceiver = new ConnectionLostReceiver();
    registerReceiver(AppService.ACTION_CONNECTION_LOST, connectionLostReceiver);
  }

  private void registerReceiver(String action, BroadcastReceiver broadcastReceiver)
  {
    IntentFilter intentFilter = new IntentFilter(action);
    intentFilter.addCategory(Intent.CATEGORY_DEFAULT);
    ContextCompat.registerReceiver(this, broadcastReceiver, intentFilter, ContextCompat.RECEIVER_NOT_EXPORTED);
  }

  public void onPublishSuccess(String topic)
  {

  }

  public void onPublishError()
  {

  }

  private void onClientNotConnected()
  {
    Intent intent = new Intent(this, AppService.class);
    intent.setAction(AppService.ACTION_STOP_FOREGROUND_SERVICE);
    startService(intent);

    DialogInterface.OnClickListener positiveListener = (dialog, which) ->
    {
      dialog.dismiss();
      finishAffinity();
    };

    AlertDialog.Builder builder = new AlertDialog.Builder(BaseActivity.this)
      .setCancelable(false)
      .setTitle(R.string.app_name)
      .setMessage(R.string.client_error)
      .setPositiveButton(R.string.accept, positiveListener);
    builder.create().show();
  }

  public class ConnectionLostReceiver extends BroadcastReceiver
  {
    public void onReceive(Context context, Intent intent)
    {
      onClientNotConnected();
    }
  }

  public class PublishFinishedReceiver extends BroadcastReceiver
  {
    @Override
    public void onReceive(Context context, Intent intent)
    {
      String topicSuccess = intent.getStringExtra(AppService.EXTRA_OF_ACTION);
      if (topicSuccess != null)
      {
        onPublishSuccess(topicSuccess);
      } else
      {
        onPublishError();
      }
    }
  }

  @Override
  protected void onDestroy()
  {
    super.onDestroy();
    if (publishFinishedReceiver != null)
    {
      unregisterReceiver(publishFinishedReceiver);
    }
    if (connectionLostReceiver != null)
    {
      unregisterReceiver(connectionLostReceiver);
    }
  }
}
