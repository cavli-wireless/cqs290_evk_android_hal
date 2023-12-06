#define LOG_TAG "vendor.cavli.cav_spi@1.0-service"

#include <vendor/cavli/hardware/spi/1.0/ISpi.h>
#include <hidl/HidlTransportSupport.h>

#include "spi.h"

// Generated HIDL files
using vendor::cavli::hardware::spi::V1_0::ISpi;
using vendor::cavli::hardware::spi::V1_0::implementation::Cavspi;

using android::sp;
using android::status_t;

int main() {
    ::android::hardware::configureRpcThreadpool(1 /*threads*/, true /*willJoin*/);

    sp spi = new Cavspi();
    const status_t status = spi->registerAsService();
    if (status != ::android::OK) {
        ALOGE("Cannot register Bluetooth HAL service");
        return 1;  // or handle error
    }

    ::android::hardware::joinRpcThreadpool();
    return 1;  // joinRpcThreadpool should never return
}
