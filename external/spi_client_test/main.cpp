#define LOG_TAG "spi_client_test"

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

#include <vendor/cavli/hardware/spi/1.0/ISpi.h>

using namespace vendor::cavli::hardware::spi::V1_0;

int main(void) {
    std::vector<uint8_t> v = {0, 1, 2, 3, 
        4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    // Initialize the ISpi service
    android::sp<ISpi> spi = ISpi::getService();
    if (spi == nullptr) {
        ALOGE("Failed to get ISpi service");
        return 1;
    }

    ALOGI("Bind to spi service success");
    
    bool ret = spi->openConnection("/dev/spidev0.0");

    ALOGI("openConnection=%s", ret?"SUCCESS":"FAILED");

    if ( ret ) {
        spi->sendData(v);
    }

    ALOGI("Close spi connection.");
    spi->closeConnection();

    // spi already a sp ( smart pointer ), don't have to delete

    return 0;
}
