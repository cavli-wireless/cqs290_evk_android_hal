package com.vendor.cavli.automotive.vehicle;

import vendor.cavli.automotive.vehicle.V1_0.IVehicle;
import vendor.cavli.automotive.vehicle.V1_0.IVehicleCallback;
import vendor.cavli.automotive.vehicle.V1_0.StatusCode;
import vendor.cavli.automotive.vehicle.V1_0.SubscribeOptions;
import vendor.cavli.automotive.vehicle.V1_0.VehiclePropConfig;
import vendor.cavli.automotive.vehicle.V1_0.VehiclePropValue;

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

public interface ICavliVehicleCallback {

    void onPropertyEvent(ArrayList<VehiclePropValue> propValues);

    void onPropertySet(VehiclePropValue propValue);

    void onPropertySetError(int errorCode, int propId, int areaId);

    void onConnected();
}