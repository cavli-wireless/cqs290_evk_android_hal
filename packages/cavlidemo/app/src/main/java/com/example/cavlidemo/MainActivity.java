package com.example.cavlidemo;

import androidx.appcompat.app.AppCompatActivity;
import vendor.cavli.hardware.spi.CavliSpi;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    CavliSpi mSpi;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ArrayList<Byte> data = new ArrayList<>();

        // Add some bytes to the vector
        data.add((byte) 10);
        data.add((byte) 20);
        data.add((byte) 30);

        mSpi = new CavliSpi();
        mSpi.openConnection("/dev/spidev0.0");
//        mSpi.sendData(data);
//        mSpi.closeConnection();

        Button copyButton = findViewById(R.id.button);
        copyButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mSpi.sendData(data);
            }
        });
    }

    @Override
    protected void onDestroy() {
        // Add your action here
        // For example, you can release resources or perform cleanup tasks
        // This method is called when the activity is about to be destroyed
        super.onDestroy();
        mSpi.closeConnection();
    }
}