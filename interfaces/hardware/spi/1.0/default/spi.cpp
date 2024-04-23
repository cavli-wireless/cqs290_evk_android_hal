#define LOG_TAG "vendor.cavli.hardware.spi@1.0-impl"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

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
    int mode = 0;
    int ret = 0;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = static_cast<unsigned int>(data.size()),
        .delay_usecs = 0,
        .speed_hz = 100000,
        .bits_per_word = 8,
    };

    if (mode & SPI_TX_QUAD)
        tr.tx_nbits = 4;
    else if (mode & SPI_TX_DUAL)
        tr.tx_nbits = 2;
    if (mode & SPI_RX_QUAD)
        tr.rx_nbits = 4;
    else if (mode & SPI_RX_DUAL)
        tr.rx_nbits = 2;
    if (!(mode & SPI_LOOP)) {
        if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
            tr.rx_buf = 0;
        else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
            tr.tx_buf = 0;
    }

    std::copy(data.begin(), data.end(), tx_buf);

    ALOGI("sendData length=%zu", data.size());
    ret = ioctl(mSpidev, SPI_IOC_MESSAGE(1), &tr);

    return true;

};

Return<int32_t> Cavspi::readData(const hidl_vec<uint8_t>& data, uint32_t length){
    (void)length;
    (void)data;
    ALOGI("readData length=%i", length);
    return 0;
};

Return<bool> Cavspi::openConnection(const hidl_string& device){
    bool ret = true;
    int i_ret = 0;

    ALOGI("openConnection device=%s", device.c_str());

    mSpidev = open(device.c_str(), O_RDWR);
    if ( mSpidev > 0 ) {
        ALOGI("spidev open success");
    } else {
        ALOGE("Can't open spidev");
        ret = false;
    }

    // Set SPI mode
    if ( ret ) {
        int mode = 0;
        i_ret = ioctl(mSpidev, SPI_IOC_WR_MODE, &mode);
        if ( -1 == i_ret ) {
            ALOGE("Set mode FAILED");
            ret = false;
        }
    }

    // Set SPI speed
    if ( ret ) {
        int speed = 100000;
        i_ret = ioctl(mSpidev, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if ( -1 == i_ret ) {
            ALOGE("Set speed FAILED");
            ret = false;
        }
    }

    // Set SPI bits per word
    if ( ret ) {
        int bpw = 8;
        i_ret = ioctl(mSpidev, SPI_IOC_WR_BITS_PER_WORD, &bpw);
        if ( -1 == i_ret ) {
            ALOGE("Set bit per word write FAILED");
            ret = false;
        }
    }

    if ( ret ) {
        int bpw = 8;
        i_ret = ioctl(mSpidev, SPI_IOC_RD_BITS_PER_WORD, &bpw);
        if ( -1 == i_ret ) {
            ALOGE("Set bit per word read FAILED");
            ret = false;
        }
    }

    tx_buf = (char *)malloc(2048);
    rx_buf = (char *)malloc(2048);

    return ret;
};

Return<bool> Cavspi::closeConnection(){
    ALOGI("closeConnection");

    if ( mSpidev > 0 ) {
        close(mSpidev);
        mSpidev = -1;
    }

    if ( rx_buf != NULL ) {
        free(rx_buf);
        rx_buf = NULL;
    }

    if ( tx_buf != NULL ) {
        free(tx_buf);
        tx_buf = NULL;
    }

    return true;
};

}  // implementation
}  // namespace V1_0
}  // namespace spi
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor
