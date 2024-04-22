package vendor.cavli.hardware.spi;

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

import java.lang.ref.WeakReference;
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

import vendor.cavli.hardware.spi.V1_0.ISpi;

public class CavliSpi {

    private static final String TAG = "CavliSpi";
    private static final boolean DEBUG = false;

    private final ISpi mSpi;

    @Nullable
    private static ISpi getSpi() {
        try {
            return ISpi.getService();
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException " + e.getMessage());
        } catch (NoSuchElementException e) {
            Log.e(TAG, "NoSuchElementException " + e.getMessage());
        }
        return null;
    }


    public CavliSpi() {
        mSpi = getSpi();
        if ( null != mSpi ) {
            Log.i(TAG, "Bind service SUCCESS");
        }
    }
    
    public boolean openConnection(String device) {
        boolean ret = false;
        try {
            ret = mSpi.openConnection(device);
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }

    public boolean closeConnection() {
        boolean ret = false;
        try {
            ret = mSpi.closeConnection();
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }
    
    public boolean sendData(java.util.ArrayList<Byte> data) {
        boolean ret = false;
        try {
            ret = mSpi.sendData(data);
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
        }
        return ret;
    }

    public int readData(java.util.ArrayList<Byte> data, int length) {
        int ret = 0;
        try {
            ret = mSpi.readData(data, length);
        } catch ( android.os.RemoteException e ) {
            Log.e(TAG, "Exceoption " + e);
            ret = -1;
        }
        return ret;
    }

}