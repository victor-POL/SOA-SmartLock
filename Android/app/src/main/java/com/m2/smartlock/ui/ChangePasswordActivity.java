package com.m2.smartlock.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.appbar.MaterialToolbar;
import com.m2.smartlock.R;
import com.m2.smartlock.service.AppService;

public class ChangePasswordActivity extends BaseActivity
{
  private static final String TAG = ChangePasswordActivity.class.getSimpleName();
  private EditText etPassword;
  private Button btnSave;
  private ProgressBar progressBar;

  @Override
  protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_change_password);
    MaterialToolbar toolbar = findViewById(R.id.toolbar);
    toolbar.setNavigationOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

    etPassword = findViewById(R.id.etPassword);
    btnSave = findViewById(R.id.btnSave);
    progressBar = findViewById(R.id.progressBar);

    setupInputPassword();
    btnSave.setOnClickListener(v -> onClickSave());

    addBroadcastReceiverForConnectionLost();
    addBroadcastReceiverForPublish();
  }

  private void setupInputPassword()
  {
    TextWatcher textWatcher = new TextWatcher()
    {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after)
      {
      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count)
      {
      }

      @Override
      public void afterTextChanged(Editable s)
      {
        btnSave.setEnabled(!TextUtils.isEmpty(s));
      }
    };
    etPassword.addTextChangedListener(textWatcher);
  }

  private void onClickSave()
  {
    String newPassword = etPassword.getText().toString();
    Log.d(TAG, "newPassword=" + newPassword);

    showLoading();
    Intent intent = new Intent(this, AppService.class);
    intent.setAction(AppService.ACTION_SET_PASS);
    intent.putExtra(AppService.EXTRA_OF_ACTION, newPassword);
    startService(intent);
  }

  private void showLoading()
  {
    progressBar.setVisibility(View.VISIBLE);
    etPassword.setEnabled(false);
    btnSave.setEnabled(false);
  }

  private void hideLoading()
  {
    progressBar.setVisibility(View.GONE);
    etPassword.setEnabled(true);
    String s = etPassword.getText().toString();
      btnSave.setEnabled(!TextUtils.isEmpty(s));
  }

  @Override
  public void onPublishSuccess(String topic)
  {
    if (topic.equals(AppService.ACTION_SET_PASS))
    {
      onSavePasswordSuccess();
    }
  }

  private void onSavePasswordSuccess()
  {
    hideLoading();
    AlertDialog.Builder builder = new AlertDialog.Builder(this)
      .setTitle(R.string.app_name)
      .setMessage(R.string.save_password_success)
      .setPositiveButton(R.string.accept, (dialog, which) -> finish());
    builder.create().show();
  }

  @Override
  public void onPublishError()
  {
    hideLoading();
    onSavePasswordError();
  }

  private void onSavePasswordError()
  {
    AlertDialog.Builder builder = new AlertDialog.Builder(this)
      .setTitle(R.string.app_name)
      .setMessage(R.string.save_password_error)
      .setPositiveButton(R.string.accept, (dialog, which) -> dialog.dismiss());
    builder.create().show();
  }
}
