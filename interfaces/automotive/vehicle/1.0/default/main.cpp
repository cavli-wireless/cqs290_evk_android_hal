#define LOG_TAG "vendor.cavli.automotive.vehicle@1.0-service"

#include <log/log.h>
#include <hidl/LegacySupport.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>

#include <vendor/cavli/automotive/vehicle/1.0/IVehicle.h>

#include "CavliVehicle.hpp"

using ::vendor::cavli::automotive::vehicle::V1_0::IVehicle;
using ::vendor::cavli::automotive::vehicle::V1_0::CavliVehicle;

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
using android::status_t;

int main(int, char *[]) {

    configureRpcThreadpool(1, true);

    sp<IVehicle> mVehicle = new CavliVehicle();
    const status_t status = mVehicle->registerAsService();
    if (status != ::android::OK) {
        return 1; // or handle error
    }

    joinRpcThreadpool(); // should never return

    //Should not pass this line
    return 1;
}
