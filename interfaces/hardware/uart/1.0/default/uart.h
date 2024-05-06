#ifndef ANDROID_HARDWARE_NFC_V1_0_NFC_H
#define ANDROID_HARDWARE_NFC_V1_0_NFC_H

#include <string>
#include <vendor/cavli/hardware/uart/1.0/IUart.h>
#include <hidl/Status.h>
#include <hardware/hardware.h>
#include <hardware/nfc.h>

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

private:

    Return<Status> transmit(const hidl_vec<uint8_t>& data);

    Return<void> receive(int32_t length, receive_cb _hidl_cb);

    Return<void> registerCallback(const sp<IUartCallback>& callback);

    Return<Status> configure(const UartConfig& config);
};


}  // implementation
}  // namespace V1_0
}  // namespace uart
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor

#endif  // ANDROID_HARDWARE_NFC_V1_0_NFC_H
