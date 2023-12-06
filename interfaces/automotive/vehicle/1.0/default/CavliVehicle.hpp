/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef vendor_cavli_automotive_vehicle_V1_0_CavliVehicle_H_
#define vendor_cavli_automotive_vehicle_V1_0_CavliVehicle_H_

#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>
#include <thread>
#include <chrono>

#include <list>
#include <map>
#include <memory>
#include <set>

#include <vendor/cavli/automotive/vehicle/1.0/IVehicle.h>
#include <vendor/cavli/automotive/vehicle/1.0/IVehicleCallback.h>
#include <vendor/cavli/automotive/vehicle/1.0/BpHwVehicleCallback.h>

namespace vendor {
namespace cavli {
namespace automotive {
namespace vehicle {
namespace V1_0 {

using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_handle;
using ::android::sp;

/**
 * This class is a thick proxy between IVehicle HIDL interface and vendor's implementation.
 *
 * It has some boilerplate code like batching and caching property values, checking permissions,
 * etc. Vendors must implement VehicleHal class.
 */
class CavliVehicle : public IVehicle {
private:
    sp<IVehicleCallback> mCallback;
    std::thread workerThread;
    int interval;
    bool stopFlag;

    void task();
public:
    CavliVehicle();
    ~CavliVehicle();
    // ---------------------------------------------------------------------------------------------
    // Methods derived from IVehicle
    Return<void> getAllPropConfigs(getAllPropConfigs_cb _hidl_cb)  override;
    Return<void> getPropConfigs(const hidl_vec<int32_t>& properties,
                                getPropConfigs_cb _hidl_cb)  override;
    Return<void> get(const VehiclePropValue& requestedPropValue,
                     get_cb _hidl_cb)  override;
    Return<StatusCode> set(const VehiclePropValue& value)  override;
    Return<StatusCode> subscribe(const sp<IVehicleCallback>& callback,
                                const hidl_vec<SubscribeOptions>& options)  override;
    Return<StatusCode> unsubscribe(const sp<IVehicleCallback>& callback,
                                   int32_t propId)  override;
    Return<void> debugDump(debugDump_cb _hidl_cb = nullptr) override;

    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& options) override;

private:

};

}  // namespace V1_0
}  // namespace vehicle
}  // namespace automotive
}  // namespace cavli
}  // namespace vendor


#endif // vendor_cavli_automotive_vehicle_V1_0_CavliVehicle_H_
