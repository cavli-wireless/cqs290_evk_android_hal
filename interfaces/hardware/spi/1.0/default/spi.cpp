#define LOG_TAG "vendor.cavli.cavli_spi@1.0-impl"

#include <log/log.h>
#include <hardware/hardware.h>

#include "spi.h"

namespace vendor {
namespace cavli {
namespace hardware {
namespace spi {
namespace V1_0 {
namespace implementation {

Return<bool> Cavspi::sendData(const hidl_vec<uint8_t>& data){
    (void)data;
    return true;

};

Return<int32_t> Cavspi::readData(const hidl_vec<uint8_t>& data, uint32_t length){
    (void)length;
    (void)data;
    return 0;
};

Return<bool> Cavspi::openConnection(const hidl_string& device){
    (void)device;
    return true;
};

Return<bool> Cavspi::closeConnection(){
    return true;
};

}  // implementation
}  // namespace V1_0
}  // namespace spi
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor
