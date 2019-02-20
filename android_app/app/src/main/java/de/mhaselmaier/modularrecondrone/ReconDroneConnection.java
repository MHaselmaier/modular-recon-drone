package de.mhaselmaier.modularrecondrone;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.util.List;

public class ReconDroneConnection extends Activity
{
    private TextView status;
    private TextView passwordLabel;
    private EditText password;
    private Button connect;

    private WifiManager wifiManager;

    private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.recon_drone_connection);

        this.status = findViewById(R.id.status);
        this.passwordLabel = findViewById(R.id.password_label);
        this.password = findViewById(R.id.password);
        this.connect = findViewById(R.id.connect);

        this.wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE);
        this.wifiManager.setWifiEnabled(true);
        removeReconDroneNetwork();
        BroadcastReceiver wifiScanReceiver = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                List<ScanResult> results = ReconDroneConnection.this.wifiManager.getScanResults();
                for (ScanResult result : results)
                {
                    if (result.SSID.equals("Recon Drone"))
                    {
                        foundReconDrone();
                        return;
                    }
                }
                lostReconDrone();
            }
        };
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        registerReceiver(wifiScanReceiver, intentFilter);

        this.connect.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                removeReconDroneNetwork();

                WifiConfiguration conf = new WifiConfiguration();
                conf.SSID = "\"Recon Drone\"";
                conf.preSharedKey = "\"" + ReconDroneConnection.this.password.getText().toString() + "\"";
                int networkID = ReconDroneConnection.this.wifiManager.addNetwork(conf);
                ReconDroneConnection.this.wifiManager.disconnect();
                ReconDroneConnection.this.wifiManager.enableNetwork(networkID, true);
                ReconDroneConnection.this.wifiManager.reconnect();

                ReconDroneConnection.this.handler.postDelayed(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        if (-1 != ReconDroneConnection.this.wifiManager.getConnectionInfo().getNetworkId())
                        {
                            Intent intent = new Intent(ReconDroneConnection.this, ReconDrone.class);
                            startActivity(intent);
                        }
                    }
                }, 250);
            }
        });

        lostReconDrone();
    }

    private void removeReconDroneNetwork()
    {
        for (WifiConfiguration i: this.wifiManager.getConfiguredNetworks())
        {
            if(i.SSID != null && i.SSID.equals("\"Recon Drone\"")) {
                this.wifiManager.removeNetwork(i.networkId);
                break;
            }
        }
    }

    private void foundReconDrone()
    {
        Log.d("result", "true");
        this.status.setText(R.string.status_found);
        this.passwordLabel.setVisibility(View.VISIBLE);
        this.password.setVisibility(View.VISIBLE);
        this.connect.setVisibility(View.VISIBLE);
    }

    private void lostReconDrone()
    {
        Log.d("result", "false");
        this.status.setText(R.string.status_searching);
        this.passwordLabel.setVisibility(View.INVISIBLE);
        this.password.setVisibility(View.INVISIBLE);
        this.connect.setVisibility(View.INVISIBLE);
    }

    @Override
    public void onResume()
    {
        super.onResume();

        searchForReconDrone();
    }

    @Override
    public void onPause()
    {
        super.onPause();

        this.handler.removeCallbacksAndMessages(null);
    }

    private void searchForReconDrone()
    {
        this.handler.removeCallbacksAndMessages(null);
        this.handler.post(new Runnable() {
            @Override
            public void run() {
                ReconDroneConnection.this.wifiManager.startScan();
                ReconDroneConnection.this.handler.postDelayed(this, 5000);
            }
        });
    }
}
