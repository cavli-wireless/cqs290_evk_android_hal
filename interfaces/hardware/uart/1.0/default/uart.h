#ifndef ANDROID_HARDWARE_NFC_V1_0_NFC_H
#define ANDROID_HARDWARE_NFC_V1_0_NFC_H

#include <string>
#include <thread>
#include <chrono>
#include <utility>
#include <condition_variable>

#include <vendor/cavli/hardware/uart/1.0/IUart.h>
#include <hidl/Status.h>
#include <hardware/hardware.h>
#include <hardware/nfc.h>

extern "C" {
    #include "fdio.h"
    #include "split.h"
    #include "term.h"
    #ifdef LINENOISE
    #include "linenoise-1.0/linenoise.h"
    #endif
    #include "custbaud.h"
}

namespace vendor {
namespace cavli {
namespace hardware {
namespace uart {
namespace V1_0 {
namespace implementation {

using ::vendor::cavli::hardware::uart::V1_0::IUart;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;

struct Cavuart : public IUart {
private:
    const int TIMEOUT = 10;
    const int BUFFER_SIZE = 1024*4;
private:
    int tty_fd;
    std::thread * listenner;
    bool keep_run;
    sp<IUartCallback> __callback = nullptr;
public:
    Cavuart();

private:
    uint32_t convert_baud(UartBaudRate baud);
    int convert_stopbits(UartStopBits stopbits);
    flowcntrl_e convert_flow(UartHardwareFlowControl flow);
    uint16_t convert_parity(UartParity parity);

    /**
     * Open
     */
    Return<Status> open_port(const hidl_string& name);

    /**
     * Close
     */
    Return<Status> close_port();

    /**
     * Transmit data over UART.
     *
     * @param data The data to be transmitted.
     * @return Returns the number of bytes successfully transmitted.
     */
    Return<Status> transmit(const hidl_vec<uint8_t>& data);

    /**
     * Receive data from UART.
     *
     * @param length The maximum length of data to receive.
     * @return Returns the received data.
     */
    Return<void> receive(int32_t length, receive_cb _hidl_cb);

    /**
     * Register a callback to receive data asynchronously.
     *
     * @param callback The callback object to be registered.
     */
    Return<void> registerCallback(const sp<IUartCallback>& callback);

    /**
     * Configure UART interface.
     *
     * @param config The configuration parameters for UART.
     */
    Return<Status> configure(const UartConfig& config);
};


}  // implementation
}  // namespace V1_0
}  // namespace uart
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor

#endif  // ANDROID_HARDWARE_NFC_V1_0_NFC_H
