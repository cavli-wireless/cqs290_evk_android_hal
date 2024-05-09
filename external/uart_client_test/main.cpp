#define LOG_TAG "uart_client_test"

#include <sys/types.h>
#include <log/log.h>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>
#include <thread>
#include <chrono>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <set>

#include <vendor/cavli/hardware/uart/1.0/IUart.h>

using namespace vendor::cavli::hardware::uart::V1_0;

int main(void) {
    std::vector<uint8_t> v = {0, 1, 2, 3, 
        4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    // Initialize the IUart service
    android::sp<IUart> uart = IUart::getService();
    if (uart == nullptr) {
        ALOGE("Failed to get IUart service");
        return 1;
    }

    ALOGI("Bind to uart service success");

    uart->open_port("/dev/ttyMSM0");

    uart->transmit(v);

    ALOGI("Close uart connection.");
    uart->close_port();

    // uart already a sp ( smart pointer ), don't have to delete

    return 0;
}
