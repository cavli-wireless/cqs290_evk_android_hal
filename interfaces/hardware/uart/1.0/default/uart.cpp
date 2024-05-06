#define LOG_TAG "vendor.cavli.hardware.uart@1.0-impl"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>

#include <log/log.h>
#include <hardware/hardware.h>

#include "uart.h"

namespace vendor {
namespace cavli {
namespace hardware {
namespace uart {
namespace V1_0 {
namespace implementation {

    Return<Status> Cavuart::transmit(const hidl_vec<uint8_t>& data) {
        (void)data;
        return Status::NO_ERROR;
    };

    Return<void> Cavuart::receive(int32_t length, receive_cb _hidl_cb) {
        (void)length;
        (void)_hidl_cb;
        return Void();
    };

    Return<void> Cavuart::registerCallback(const sp<IUartCallback>& callback) {
        (void)callback;
        return Void();
    };

    Return<Status> Cavuart::configure(const UartConfig& config) {
        (void)config;
        return Status::NO_ERROR;
    };

}  // implementation
}  // namespace V1_0
}  // namespace uart
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor
