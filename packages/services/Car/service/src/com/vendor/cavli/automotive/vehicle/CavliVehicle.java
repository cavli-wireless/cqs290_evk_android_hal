package com.vendor.cavli.automotive.vehicle;

import static android.os.SystemClock.elapsedRealtime;

import vendor.cavli.automotive.vehicle.V1_0.IVehicle;
import vendor.cavli.automotive.vehicle.V1_0.IVehicleCallback;
import vendor.cavli.automotive.vehicle.V1_0.StatusCode;
import vendor.cavli.automotive.vehicle.V1_0.SubscribeOptions;
import vendor.cavli.automotive.vehicle.V1_0.VehiclePropConfig;
import vendor.cavli.automotive.vehicle.V1_0.VehiclePropValue;

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

import vendor.cavli.automotive.vehicle.V1_0.IVehicle;
import vendor.cavli.automotive.vehicle.V1_0.IVehicleCallback;

import com.vendor.cavli.automotive.vehicle.ICavliVehicleCallback;

public class CavliVehicle {

    private static final String TAG = "CavliVehicle";
    private static final boolean DEBUG = false;

    private final IVehicle mVehicle;
    private final IVehicleCallback mInternalCallback;

    private final int mWaitCapMs = 100;
    private final int mSleepMs = 10;

    @Nullable
    private static IVehicle getVehicle() {
        try {
            return IVehicle.getService();
        } catch (RemoteException e) {

        } catch (NoSuchElementException e) {

        }

        return null;
    }


    public CavliVehicle(Looper looper, ICavliVehicleCallback callback) {
        mVehicle = getVehicle();
        Handler handler = new CallbackHandler(looper, callback);
        mInternalCallback = new VehicleCallback(handler);
    }

    public ArrayList<VehiclePropConfig> getAllPropConfigs() throws RemoteException {
        // Implement the logic to retrieve all property configurations
        return mVehicle.getAllPropConfigs();
    }


    public void subscribe(SubscribeOptions... options) throws RemoteException {
        mVehicle.subscribe(mInternalCallback, new ArrayList<>(Arrays.asList(options)));
    }

    public void unsubscribe(int prop) throws RemoteException {
        mVehicle.unsubscribe(mInternalCallback, prop);
    }


    public void setValue(VehiclePropValue propValue) {
        int status = invokeRetriable(() -> {
            try {
                return mVehicle.set(propValue);
            } catch (RemoteException e) {
                Log.e(TAG, getValueErrorMessage("set", propValue), e);
                return StatusCode.TRY_AGAIN;
            }
        }, mWaitCapMs, mSleepMs);

        if (StatusCode.INVALID_ARG == status) {
            throw new IllegalArgumentException(getValueErrorMessage("set", propValue));
        }

        if (StatusCode.OK != status) {
            Log.e(TAG, getPropertyErrorMessage("set", propValue, status));
            throw new ServiceSpecificException(status,
                    "Failed to set property: 0x" + Integer.toHexString(propValue.prop)
                            + " in areaId: 0x" + Integer.toHexString(propValue.areaId));
        }
    }

    VehiclePropValue getValue(VehiclePropValue requestedPropValue) {
        final ObjectWrapper<VehiclePropValue> valueWrapper = new ObjectWrapper<>();
        int status = invokeRetriable(() -> {
            ValueResult res = internalGet(requestedPropValue);
            valueWrapper.object = res.propValue;
            return res.status;
        }, mWaitCapMs, mSleepMs);

        if (StatusCode.INVALID_ARG == status) {
            throw new IllegalArgumentException(getValueErrorMessage("get", requestedPropValue));
        }

        if (StatusCode.OK != status || valueWrapper.object == null) {
            // If valueWrapper.object is null and status is StatusCode.Ok, change the status to be
            // NOT_AVAILABLE.
            if (StatusCode.OK == status) {
                status = StatusCode.NOT_AVAILABLE;
            }
            Log.e(TAG, getPropertyErrorMessage("get", requestedPropValue, status));
            throw new ServiceSpecificException(status,
                    "Failed to get property: 0x" + Integer.toHexString(requestedPropValue.prop)
                            + " in areaId: 0x" + Integer.toHexString(requestedPropValue.areaId));
        }

        return valueWrapper.object;
    }

    private String getValueErrorMessage(String action, VehiclePropValue propValue) {
        return String.format("Failed to %s value for: 0x%s, areaId: 0x%s", action,
                Integer.toHexString(propValue.prop), Integer.toHexString(propValue.areaId));
    }

    private String getPropertyErrorMessage(String action, VehiclePropValue propValue, int status) {
        return String.format("Failed to %s property: 0x%s, areaId: 0x%s, code: %d (%s)", action,
                Integer.toHexString(propValue.prop), Integer.toHexString(propValue.areaId),
                status, StatusCode.toString(status));
    }

    private ValueResult internalGet(VehiclePropValue requestedPropValue) {
        final ValueResult result = new ValueResult();
        try {
            mVehicle.get(requestedPropValue,
                    (status, propValue) -> {
                        result.status = status;
                        result.propValue = propValue;
                    });
        } catch (RemoteException e) {
            Log.e(TAG, getValueErrorMessage("get", requestedPropValue), e);
            result.status = StatusCode.TRY_AGAIN;
        }

        return result;
    }

    interface RetriableCallback {
        /** Returns {@link StatusCode} */
        int action();
    }

    private static int invokeRetriable(RetriableCallback callback, long timeoutMs, long sleepMs) {
        int status = callback.action();
        long startTime = elapsedRealtime();
        while (StatusCode.TRY_AGAIN == status && (elapsedRealtime() - startTime) < timeoutMs) {
            if (DEBUG) {
                Log.d(TAG, "Status before sleeping " + sleepMs + "ms: "
                        + StatusCode.toString(status));
            }
            try {
                Thread.sleep(sleepMs);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                Log.e(TAG, "Thread was interrupted while waiting for vehicle HAL.", e);
                break;
            }

            status = callback.action();
            if (DEBUG) Log.d(TAG, "Status after waking up: " + StatusCode.toString(status));
        }
        if (DEBUG) Log.d(TAG, "Returning status: " + StatusCode.toString(status));
        return status;
    }

    private static final class ObjectWrapper<T> {
        T object;
    }

    private static final class ValueResult {
        int status;
        VehiclePropValue propValue;
    }

    private static final class PropertySetError {
        final int errorCode;
        final int propId;
        final int areaId;

        PropertySetError(int errorCode, int propId, int areaId) {
            this.errorCode = errorCode;
            this.propId = propId;
            this.areaId = areaId;
        }
    }

    private static final class CallbackHandler extends Handler {
        private static final int MSG_ON_PROPERTY_SET = 1;
        private static final int MSG_ON_PROPERTY_EVENT = 2;
        private static final int MSG_ON_SET_ERROR = 3;

        private final WeakReference<ICavliVehicleCallback> mCallback;

        CallbackHandler(Looper looper, ICavliVehicleCallback callback) {
            super(looper);
            mCallback = new WeakReference<ICavliVehicleCallback>(callback);
        }

        @Override
        public void handleMessage(Message msg) {
            ICavliVehicleCallback callback = mCallback.get();
            if (callback == null) {
                Log.i(TAG, "handleMessage null callback");
                return;
            }

            switch (msg.what) {
                case MSG_ON_PROPERTY_EVENT:
                    callback.onPropertyEvent((ArrayList<VehiclePropValue>) msg.obj);
                    break;
                case MSG_ON_PROPERTY_SET:
                    callback.onPropertySet((VehiclePropValue) msg.obj);
                    break;
                case MSG_ON_SET_ERROR:
                    PropertySetError obj = (PropertySetError) msg.obj;
                    callback.onPropertySetError(obj.errorCode, obj.propId, obj.areaId);
                    break;
                default:
                    Log.e(TAG, "Unexpected message: " + msg.what);
            }

        }
    }

    private static final class VehicleCallback extends IVehicleCallback.Stub {
        private final Handler mHandler;

        VehicleCallback(Handler handler) {
            mHandler = handler;
        }

        @Override
        public void onPropertyEvent(ArrayList<VehiclePropValue> propValues) {
            mHandler.sendMessage(Message.obtain(
                    mHandler, CallbackHandler.MSG_ON_PROPERTY_EVENT, propValues));
        }

        @Override
        public void onPropertySet(VehiclePropValue propValue) {
            mHandler.sendMessage(Message.obtain(
                    mHandler, CallbackHandler.MSG_ON_PROPERTY_SET, propValue));
        }

        @Override
        public void onPropertySetError(int errorCode, int propId, int areaId) {
            mHandler.sendMessage(Message.obtain(
                    mHandler, CallbackHandler.MSG_ON_SET_ERROR,
                    new PropertySetError(errorCode, propId, areaId)));
        }
    }

}