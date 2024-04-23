# DEMO
The following demo instructs you on how to use I2C/SPI/UART from an Android Application. Some information needs to be clarified as follows:

Android application: We understand that the Android app is written in Java/Kotlin/C/C++. Android is built from Android Studio or built along with the source code.
Application: Using peripherals from an Android app is rare and usually serves specific purposes such as creating an IPC channel between chips on the board, integrating existing software/stacks on other platforms, etc. If you need to use I2C/SPI/UART to communicate with peripherals or sensors, it's advisable to use a different approach.


# How to run DEMO

## Requirements
  - Your board must have been flashed with metabuild/flatbuild. You can use fastboot for this.
  - This demo has been verified when built with the following targets:
    + Vendor: bengal_2w
    + System: qssi_12
  - Other targets have not been validated yet. The writer expects you to have a similar setup to minimize any arising issues.

## Running the demo with the prebuild images
  - Step 1: Obtain images and flash with fastboot.
  - Step 2: Connect the oscilloscope or logic analyzer to the SPI pins.
  - Step 3: Run the native C demo. Execute the following command:
    ```
    spi_client_test
    ```
  - Step 4: Check the results on the oscilloscope. You can refer to the following video.
  - Step 5: Run the Java/Kotlin demo. Open the app with the following command:
    ```
    adb shell am start -n com.example.demo/.MainActivity
    ```
  - Step 6: Press the Run button on the screen and observe the results on the oscilloscope.

# How to build demo

## Requirements
  - Before building this demo, I require you to have successfully built the Android source released by Cavli.
  - Please refer below this [wiki](https://github.com/cavli-wireless/CQS290/wiki) 

## Instructions for building
  There are 2 ways to build: using repo to sync full or only cloning additional demo repo.

### Method 1: Only clone the demo repo

  - Step 1: Clone the demo [repo](https://github.com/cavli-wireless/valeo_cqs290_android_hal) into the following locations for both projects, bengal and qssi_12:
    ```
    # bengal_2w
    cd vendor
    git clone https://github.com/cavli-wireless/valeo_cqs290_android_hal -b common/demo_spi
    # rename to common ( mandatory )
    mv valeo_cqs290_android_hal common
    # similarly for qssi_12
    ```
  - Step 2 : Apply patches
    ```
    # top of bengal_2w
    cd vendor
    patch -p1 < common/patches/0001-compatibility-Add-new-HAL-to-fw-matrix.patch
    cd -
    cd device/qcom
    patch -p1 < ../../vendor/common/patches/0001-build-include-custom-target.patch
    cd -
    # top of qssi_12
    cd vendor
    patch -p1 < common/patches/0001-compatibility-qssi_12-Add-new-HAL-to-fw-matrix.patch 
    # qssi_12 only needs to update the compatibility framework, only vendor needs to add device.
    ```
  Step 3: Build bengal_2w + qssi_12, use the script to generate super images, and you're done.
