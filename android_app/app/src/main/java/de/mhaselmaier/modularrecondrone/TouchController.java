package de.mhaselmaier.modularrecondrone;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class TouchController implements View.OnTouchListener
{
    private enum SIDE_OF_SCREEN
    {
        LEFT, RIGHT
    }

    private Context context;

    private volatile int acceleration = -1;
    private volatile int steering = -1;

    private float[] startingPositions = new float[2];
    private float[] currentPositions = new float[2];

    public TouchController(Context context)
    {
        this.context = context;
        sendControllerUpdate();
    }

    @Override
    public boolean onTouch(View view, MotionEvent event)
    {
        final int pointerID = event.getPointerId(event.getActionIndex());
        switch (event.getActionMasked())
        {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN:
            if (SIDE_OF_SCREEN.RIGHT == getSideOfScreen(event.getX(pointerID)))
            {
                if (this.acceleration == -1)
                {
                    this.acceleration = pointerID;
                    this.startingPositions[0] = event.getY(pointerID);
                }
                else if (this.steering == -1)
                {
                    this.steering = pointerID;
                    this.startingPositions[1] = event.getX(pointerID);
                }
            }
            else
            {
                if (this.steering == -1)
                {
                    this.steering = pointerID;
                    this.startingPositions[1] = event.getX(pointerID);
                }
                else if (this.acceleration == -1)
                {
                    this.acceleration = pointerID;
                    this.startingPositions[0] = event.getY(pointerID);
                }
            }
            break;
        case MotionEvent.ACTION_MOVE:
            for (int i = 0; event.getPointerCount() > i; ++i)
            {
                if (event.getPointerId(i) == this.acceleration)
                {
                    this.currentPositions[0] = event.getY(i);
                }
                if (event.getPointerId(i) == this.steering)
                {
                    this.currentPositions[1] = event.getX(i);
                }
            }
            break;
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
        case MotionEvent.ACTION_CANCEL:
            if (this.acceleration == pointerID)
            {
                this.acceleration = -1;
                this.startingPositions[0] = 0;
            }
            if (this.steering == pointerID)
            {
                this.steering = -1;
                this.startingPositions[1] = 0;
            }

            view.performClick(); // For compatibility with accessibility service
            break;
        }

        if (-1 != this.acceleration)
        {
            Log.d("info", "acceleration: " + (this.startingPositions[0] - this.currentPositions[0]) + ", " + (this.startingPositions[0] - this.currentPositions[0]));
        }
        if (-1 != this.steering)
        {
            Log.d("info", "steering: " + (this.startingPositions[1] - this.currentPositions[1]) + ", " + (this.startingPositions[1] - this.currentPositions[1]));
        }

        return true;
    }

    private SIDE_OF_SCREEN getSideOfScreen(float x)
    {
        Display display = ((Activity)this.context).getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        int width = size.x;

        return (width / 2. > x ? SIDE_OF_SCREEN.LEFT : SIDE_OF_SCREEN.RIGHT);
    }

    private void sendControllerUpdate()
    {
        new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                try
                {
                    DatagramSocket socket = new DatagramSocket();
                    socket.setBroadcast(true);

                    while(true)
                    {
                        byte[] buffer = new byte[2];
                        setAcceleration(buffer);
                        setSteering(buffer);
                        setDirection(buffer);
                        adjustForInPlaceTurn(buffer);

                        Log.d("buffer123", buffer[0] + " " + buffer[1]);

                        try
                        {
                            DatagramPacket packet = new DatagramPacket(buffer, buffer.length,
                                    InetAddress.getByName(ReconDrone.ESP_HOST_ADDRESS), 4242);
                            socket.send(packet);
                            Log.d("modular-recon-drone", "send");
                        }
                        catch (Exception e)
                        {
                            Log.e("modular-recon-drone", e.toString());
                        }

                        Thread.sleep(50);
                    }
                }
                catch (Exception e)
                {
                    Log.e("modular-recon-drone", e.getMessage());
                }
            }
        }).start();
    }

    private void setAcceleration(byte[] buffer)
    {
        if (TouchController.this.acceleration != -1)
        {
            float delta = Math.min(Math.abs(TouchController.this.startingPositions[0] - TouchController.this.currentPositions[0]) / 4, 50) * 0.8f;
            buffer[0] = (byte)(50 + delta);
            buffer[1] = (byte)(50 + delta);
        }
    }

    private void setSteering(byte[] buffer)
    {
        if (TouchController.this.steering != -1)
        {
            float delta = (TouchController.this.startingPositions[1] - TouchController.this.currentPositions[1]) / 4;
            if (delta < -40)
            {
                buffer[1] *= .5;
            }
            else if (delta < -30)
            {
                buffer[1] *= .6;
            }
            else if (delta < -20)
            {
                buffer[1] *= .7;
            }
            else if (delta < -10)
            {
                buffer[1] *= .8;
            }
            else if (delta > 40)
            {
                buffer[0] *= .5;
            }
            else if (delta > 30)
            {
                buffer[0] *= .6;
            }
            else if (delta > 20)
            {
                buffer[0] *= .7;
            }
            else if (delta > 10)
            {
                buffer[0] *= .8;
            }
        }
    }

    private void setDirection(byte[] buffer)
    {
        if (TouchController.this.acceleration != -1)
        {
            float delta = TouchController.this.startingPositions[0] - TouchController.this.currentPositions[0];
            if (delta < 0)
            {
                buffer[0] |= 1 << 7;
                buffer[1] |= 1 << 7;
            }
        }
    }

    private void adjustForInPlaceTurn(byte[] buffer)
    {
        if (TouchController.this.acceleration == -1 && TouchController.this.steering != -1)
        {
            float delta = TouchController.this.startingPositions[1] - TouchController.this.currentPositions[1];
            if (delta > 0)
            {
                buffer[1] = 60;
                buffer[0] = (byte)(60 | 1 << 7);
            }
            else
            {
                buffer[0] = 60;
                buffer[1] = (byte)(60 | 1 << 7);
            }
        }
    }
}
