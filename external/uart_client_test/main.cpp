#define LOG_TAG "uart_client_test"

#include <sys/types.h>
#include <log/log.h>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>
#include <chrono>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <set>
#include <condition_variable>
#include <mutex>

#include <vendor/cavli/hardware/uart/1.0/IUart.h>

using namespace vendor::cavli::hardware::uart::V1_0;

static std::mutex mtx;
static std::condition_variable cv;
static bool ready=false;

static UartBaudRate convert_baud(int baud) {
    UartBaudRate ret = UartBaudRate::BAUD_115200;
    switch (baud)
    {
    case 1200:
        ret = UartBaudRate::BAUD_1200;
        break;
    case 2400:
        ret = UartBaudRate::BAUD_2400;
        break;
    case 4800:
        ret = UartBaudRate::BAUD_4800;
        break;
    case 9600:
        ret = UartBaudRate::BAUD_9600;
        break;
    case 19200:
        ret = UartBaudRate::BAUD_19200;
        break;
    case 38400:
        ret = UartBaudRate::BAUD_38400;
        break;
    case 115200:
        ret = UartBaudRate::BAUD_115200;
        break;
    default:
        break;
    }
    return ret;
}

class UartCallback : public IUartCallback {
public:
    // Implementation of onDataReceived method
    ::android::hardware::Return<void> onDataReceived(const android::hardware::hidl_vec<uint8_t>& data) override {
        ALOGI("onDataReceived %s", (const char *)data.data());
        ready = true;
        cv.notify_one();
        return ::android::hardware::Void();
    }
};

int main(int argc, const char ** argv) {

    UartConfig cfg;
    std::vector<uint8_t> v(argv[3], argv[3] + strlen(argv[3]));
    (void)argc;
    android::sp<IUartCallback> cb = new UartCallback();

    // Initialize the IUart service
    android::sp<IUart> uart = IUart::getService();
    if (uart == nullptr) {
        ALOGE("Failed to get IUart service");
        return 1;
    }

    ALOGI("dev=%s baud=%i", argv[1], (int)convert_baud(atoi(argv[2])));

    uart->open_port(argv[1]);

    uart->registerCallback( cb );

    cfg.baudRate = convert_baud(atoi(argv[2]));
    cfg.hardwareFlowControl = UartHardwareFlowControl::NONE;
    cfg.stopBits = UartStopBits::ONE;
    cfg.parity = UartParity::NONE;

    uart->configure(cfg);

    uart->transmit(v);

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; });

    uart->close_port();

    return 0;
}
