package de.mhaselmaier.modularrecondrone;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.Touch;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;

public class ReconDrone extends Activity
{
    public static final String ESP_HOST_ADDRESS = "192.168.4.1";

    private TouchController controller;
    private CameraFeed feed;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        enableFullScreen();
        setContentView(R.layout.recon_drone);

        ImageView videoStream = findViewById(R.id.video_stream);
        this.controller = new TouchController(this);
        videoStream.setOnTouchListener(this.controller);

        this.feed = new CameraFeed(videoStream);
        this.feed.startFeed();
    }

    private void enableFullScreen()
    {
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus)
        {
            enableFullScreen();
        }
    }
}
