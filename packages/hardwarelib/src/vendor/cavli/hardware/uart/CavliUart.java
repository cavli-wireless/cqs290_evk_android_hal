package vendor.cavli.hardware.uart;

import static android.os.SystemClock.elapsedRealtime;

import android.annotation.Nullable;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.database.Cursor;
import android.net.Uri;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.os.UserHandle;
import android.os.ServiceSpecificException;
import android.os.RemoteException;
import android.provider.TimeZoneRulesDataContract;
import android.util.Log;
import android.util.Pair;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.NoSuchElementException;

import vendor.cavli.hardware.uart.V1_0.IUart;
import vendor.cavli.hardware.uart.V1_0.IUartCallback;
import vendor.cavli.hardware.uart.V1_0.UartConfig;

import vendor.cavli.hardware.uart.ICavliUartCallback;

public class CavliUart {

    private static final String TAG = "CavliUart";
    private final IUartCallback mInternalCallback;

    private final IUart mUart;

    @Nullable
    private static IUart getUart() {
        try {
            return IUart.getService();
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException " + e.getMessage());
        } catch (NoSuchElementException e) {
            Log.e(TAG, "NoSuchElementException " + e.getMessage());
        }
        return null;
    }


    public CavliUart(Looper looper, ICavliUartCallback callback) {
        mUart = getUart();
        Log.i(TAG, "Bind service SUCCESS");
        Handler handler = new CallbackHandler(looper, callback);
        mInternalCallback = new UartCallback(handler);

    }

    public int open_port(String device) {
        int ret = -1;
        try {
            ret = mUart.open_port(device);
            if ( 0 == ret ) {
                mUart.registerCallback(mInternalCallback);
            }
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }

    public int close_port() {
        int ret = -1;
        try {
            ret = mUart.close_port();
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exception " + e);
        }
        return ret;
    }

    public int transmit(java.util.ArrayList<Byte> data) {
        int ret = -1;
        try {
            ret = mUart.transmit(data);
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }

    public int configure(UartConfig config) {
        int ret = -1;
        try {
            ret = mUart.configure(config);
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }


    private static final class CallbackHandler extends Handler {
        private static final int MSG_ON_DATA_RECV = 1;
        private final ICavliUartCallback mCallback;

        CallbackHandler(Looper looper, ICavliUartCallback callback) {
            super(looper);
            mCallback = callback; // strong reference
        }

        @Override
        public void handleMessage(Message msg) {
            if (mCallback == null) {
                Log.i(TAG, "handleMessage null callback");
                return;
            }

            try {
                switch (msg.what) {
                    case MSG_ON_DATA_RECV:
                        mCallback.onDataReceived((ArrayList<Byte>) msg.obj);
                        break;
                    default:
                        Log.e(TAG, "Unexpected message: " + msg.what);
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Message failed: " + msg.what);
            }
        }
    }

    private static final class UartCallback extends IUartCallback.Stub {
        private final Handler mHandler;
        private static final String TAG = "UartCallback";

        public UartCallback(Handler handler) {
            mHandler = handler;
        }

        @Override
        public void onDataReceived(ArrayList<Byte> arrayList) throws RemoteException {
            mHandler.sendMessage(Message.obtain(
                mHandler, CallbackHandler.MSG_ON_DATA_RECV, arrayList));
        }

    }

}
