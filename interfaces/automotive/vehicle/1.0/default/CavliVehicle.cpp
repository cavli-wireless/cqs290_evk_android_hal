#define LOG_TAG "vendor.cavli.automotive.vehicle@1.0-impl"

#include <cmath>
#include <fstream>

#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <android/log.h>

#include "CavliVehicle.hpp"

namespace vendor {
namespace cavli {
namespace automotive {
namespace vehicle {
namespace V1_0 {

Return<void> CavliVehicle::getAllPropConfigs(getAllPropConfigs_cb _hidl_cb) {

    return Void();
};

Return<void> CavliVehicle::getPropConfigs(const hidl_vec<int32_t>& properties,
                            getPropConfigs_cb _hidl_cb) {
    return Void();
};

Return<void> CavliVehicle::get(const VehiclePropValue& requestedPropValue,
                    get_cb _hidl_cb) {
    return Void();
};

Return<StatusCode> CavliVehicle::set(const VehiclePropValue& value) {
    return StatusCode::OK;
};

Return<StatusCode> CavliVehicle::subscribe(const sp<IVehicleCallback>& callback,
                            const hidl_vec<SubscribeOptions>& options) {
    return StatusCode::OK;
};

Return<StatusCode> CavliVehicle::unsubscribe(const sp<IVehicleCallback>& callback,
                                int32_t propId) {
    return StatusCode::OK;
};

Return<void> CavliVehicle::debugDump(debugDump_cb _hidl_cb ) {
    return Void();
};


Return<void> CavliVehicle::debug(const hidl_handle& fd, const hidl_vec<hidl_string>& options) {
    return Void();
};


}  // namespace V1_0
}  // namespace vehicle
}  // namespace automotive
}  // namespace cavli
}  // namespace vendor
