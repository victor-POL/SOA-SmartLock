package com.m2.smartlock.ui.shake;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.m2.smartlock.R;
import com.m2.smartlock.ui.MainActivity;

public class ShakeSuccessActivity extends AppCompatActivity
{
  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_shake_success);

    findViewById(R.id.btnRetry).setOnClickListener(v -> onClickRetry());
    findViewById(R.id.btnHome).setOnClickListener(v -> onClickHome());
  }

  private void onClickRetry()
  {
    getOnBackPressedDispatcher().onBackPressed();
  }

  private void onClickHome()
  {
    Intent intent = new Intent(this, MainActivity.class);
    startActivity(intent);
    finishAffinity();
  }
}
