#define LOG_TAG "vendor.cavli.automotive.vehicle@1.0-impl"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>

#include <android/log.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>

#include "CavliVehicle.hpp"

namespace vendor {
namespace cavli {
namespace automotive {
namespace vehicle {
namespace V1_0 {

using namespace vendor::cavli::automotive::vehicle::V1_0;

template<typename T>
void shallowCopyHidlVec(hidl_vec <T>* dest, const hidl_vec <T>& src) {
    if (src.size() > 0) {
        dest->setToExternal(const_cast<T*>(&src[0]), src.size());
    } else if (dest->size() > 0) {
        dest->resize(0);
    }
}

void shallowCopyHidlStr(hidl_string* dest, const hidl_string& src) {
    if (src.empty()) {
        dest->clear();
    } else {
        dest->setToExternal(src.c_str(), src.size());
    }
}

float generateRandomFloat(float A, float B) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(A, B);

    return distribution(gen);
}

CavliVehicle::CavliVehicle() : mCallback{nullptr}, interval{1000} {
    // Start the worker thread when the object is created
    
    
    workerThread = std::thread([this] {
        ALOGI("Simulate is running");
        float odo_meter = 0.0f;
        while (!stopFlag) {
            // Execute the task
            {
                if ( mCallback ) {
                    hidl_vec<VehiclePropValue> vec;
                    vec.resize(2);
                    vec[0].prop = static_cast<int32_t>(VehicleProperty::PERF_ODOMETER);
                    vec[0].areaId = static_cast<int32_t>(VehicleArea::GLOBAL);
                    vec[0].value.floatValues.resize(1);
                    odo_meter+=generateRandomFloat(100, 200);
                    vec[0].value.floatValues = hidl_vec<float_t>{odo_meter};

                    vec[1].prop = static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED);
                    vec[1].areaId = static_cast<int32_t>(VehicleArea::GLOBAL);
                    vec[1].value.floatValues.resize(1);
                    vec[1].value.floatValues = hidl_vec<float_t>{generateRandomFloat(100, 200)};

                    mCallback->onPropertyEvent(vec);

                } else {
                    ALOGI("Not one register, do nothing");
                }
            }
            // Sleep for the specified interval
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    });
};

CavliVehicle::~CavliVehicle() {
    // Stop the worker thread when the object is destructed
    stopFlag = true;
    if (workerThread.joinable()) {
        workerThread.join();
    }
};



Return<void> CavliVehicle::getAllPropConfigs(getAllPropConfigs_cb _hidl_cb) {

    ALOGI("CavliVehicle::getAllPropConfigs");
    return Void();
};

Return<void> CavliVehicle::getPropConfigs(const hidl_vec<int32_t>& properties,
                            getPropConfigs_cb _hidl_cb) {
    ALOGI("CavliVehicle::getPropConfigs");
    return Void();
};

Return<void> CavliVehicle::get(const VehiclePropValue& requestedPropValue,
                    get_cb _hidl_cb) {
    ALOGI("CavliVehicle::get");
    return Void();
};

Return<StatusCode> CavliVehicle::set(const VehiclePropValue& value) {
    ALOGI("CavliVehicle::set");
    return StatusCode::OK;
};

Return<StatusCode> CavliVehicle::subscribe(const sp<IVehicleCallback>& callback,
                            const hidl_vec<SubscribeOptions>& options) {
    ALOGI("CavliVehicle::subscribe");
    mCallback = std::move(callback);
    return StatusCode::OK;
};

Return<StatusCode> CavliVehicle::unsubscribe(const sp<IVehicleCallback>& callback,
                                int32_t propId) {
    ALOGI("CavliVehicle::unsubscribe");
    mCallback = nullptr;
    return StatusCode::OK;
};

Return<void> CavliVehicle::debugDump(debugDump_cb _hidl_cb ) {
    ALOGI("CavliVehicle::debugDump");
    return Void();
};


Return<void> CavliVehicle::debug(const hidl_handle& fd, 
                            const hidl_vec<hidl_string>& options) {
    ALOGI("CavliVehicle::debug");
    return Void();
};

}  // namespace V1_0
}  // namespace vehicle
}  // namespace automotive
}  // namespace cavli
}  // namespace vendor
