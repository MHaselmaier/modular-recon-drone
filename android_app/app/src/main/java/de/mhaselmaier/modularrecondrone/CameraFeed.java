package de.mhaselmaier.modularrecondrone;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

public class CameraFeed
{
    private static final int IMAGE_WIDTH = 160;
    private static final int IMAGE_HEIGHT = 120;
    private static final Matrix ROTATION_MATRIX;
    static
    {
        ROTATION_MATRIX = new Matrix();
        ROTATION_MATRIX.postRotate(180);
    }

    private ImageView imageView;
    private Handler handler = new Handler();

    private Thread backgroundThread;

    public CameraFeed(ImageView imageView)
    {
        this.imageView = imageView;
    }

    public void startFeed()
    {
        if (null != this.backgroundThread)
        {
            this.backgroundThread.interrupt();
        }

        this.backgroundThread = new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                while (true)
                {
                    try (Socket socket = new Socket(ReconDrone.ESP_HOST_ADDRESS, 1234))
                    {
                        DataInputStream inputStream = new DataInputStream(socket.getInputStream());

                        while (true)
                        {
                            final byte receivedImage[] = new byte[IMAGE_WIDTH * IMAGE_HEIGHT];
                            inputStream.readFully(receivedImage);


                            CameraFeed.this.handler.post(new Runnable()
                            {
                                @Override
                                public void run()
                                {
                                    byte modifiedImage[] = new byte[IMAGE_WIDTH * IMAGE_HEIGHT * 4];
                                    for (int i = 0; receivedImage.length > i; ++i)
                                    {
                                        modifiedImage[i * 4] = receivedImage[i];
                                        modifiedImage[i * 4 + 1] = receivedImage[i];
                                        modifiedImage[i * 4 + 2] = receivedImage[i];
                                        modifiedImage[i * 4 + 3] = (byte) 255;

                                    }

                                    Bitmap bitmap = Bitmap.createBitmap(IMAGE_WIDTH, IMAGE_HEIGHT, Bitmap.Config.ARGB_8888);
                                    bitmap.copyPixelsFromBuffer(ByteBuffer.wrap(modifiedImage));
                                    bitmap = Bitmap.createBitmap(bitmap, 0, 0, CameraFeed.IMAGE_WIDTH,
                                            CameraFeed.IMAGE_HEIGHT, CameraFeed.ROTATION_MATRIX, true);
                                    CameraFeed.this.imageView.setImageBitmap(bitmap);
                                }
                            });
                        }
                    }
                    catch (IOException e)
                    {
                        Log.e("CameraFeed", e.getMessage());
                    }
                }
            }
        });
        this.backgroundThread.start();
    }
}
