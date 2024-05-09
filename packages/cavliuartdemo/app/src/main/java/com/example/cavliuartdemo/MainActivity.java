package com.example.cavliuartdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.hidl.base.V1_0.DebugInfo;
import android.os.Bundle;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import vendor.cavli.hardware.uart.CavliUart;
import vendor.cavli.hardware.uart.ICavliUartCallback;
import vendor.cavli.hardware.uart.V1_0.IUartCallback;
import vendor.cavli.hardware.uart.V1_0.UartBaudRate;
import vendor.cavli.hardware.uart.V1_0.UartConfig;
import vendor.cavli.hardware.uart.V1_0.UartHardwareFlowControl;
import vendor.cavli.hardware.uart.V1_0.UartParity;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {
    CavliUart uart;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        uart = new CavliUart(Looper.getMainLooper(), new ICavliUartCallback() {
            @Override
            public void onDataReceived(ArrayList<Byte> arrayList) throws RemoteException {
                Log.i("CavDemoApp", "Recv " + arrayList);
            }
        });

        UartConfig config = new UartConfig();
        uart.open_port("/dev/ttyMSM0");
        config.baudRate = UartBaudRate.BAUD_115200;
        config.parity = UartParity.NONE;
        config.hardwareFlowControl = UartHardwareFlowControl.NONE;
        uart.configure(config);


        Button a = findViewById(R.id.button);
        a.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String input = "0123456789ABCDEF";
                ArrayList<Byte> arrayList = new ArrayList<>();
                input.chars().forEach(c -> arrayList.add((byte) c));
                uart.transmit(arrayList);
            }
        });


    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        uart.close_port();
    }
}