#define LOG_TAG "vendor.cavli.cav_uart@1.0-service"

#include <vendor/cavli/hardware/uart/1.0/IUart.h>
#include <hidl/HidlTransportSupport.h>

#include "uart.h"

// Generated HIDL files
using vendor::cavli::hardware::uart::V1_0::IUart;
using vendor::cavli::hardware::uart::V1_0::implementation::Cavuart;

using android::sp;
using android::status_t;

int main() {
    ::android::hardware::configureRpcThreadpool(1 /*threads*/, true /*willJoin*/);

    sp<IUart> uart = new Cavuart();
    const status_t status = uart->registerAsService();
    if (status != ::android::OK) {
        ALOGE("Cannot register Bluetooth HAL service");
        return 1;  // or handle error
    }

    ::android::hardware::joinRpcThreadpool();
    return 1;  // joinRpcThreadpool should never return
}
