#define LOG_TAG "vendor.cavli.hardware.uart@1.0-impl"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <linux/ioctl.h>
#include <linux/types.h>

#include <log/log.h>
#include <hardware/hardware.h>

#include <string>
#include <thread>
#include <chrono>
#include <utility>
#include <condition_variable>

#ifndef TIOCPMGET
#define TIOCPMGET 0x544D /* PM get */
#endif
#ifndef TIOCPMPUT
#define TIOCPMPUT 0x544E /* PM put */
#endif
#ifndef TIOCPMACT
#define TIOCPMACT 0x544F /* PM is active */
#endif

#define USERIAL_OP_CLK_ON       TIOCPMGET   /* PM get */
#define USERIAL_OP_CLK_OFF      TIOCPMPUT   /* PM put */
#define USERIAL_OP_CLK_STATE    TIOCPMACT   /* PM is active */

extern "C" {
    #include "fdio.h"
    #include "split.h"
    #include "term.h"
    #ifdef LINENOISE
    #include "linenoise-1.0/linenoise.h"
    #endif
    #include "custbaud.h"
}

#include "uart.h"

namespace vendor {
namespace cavli {
namespace hardware {
namespace uart {
namespace V1_0 {
namespace implementation {

    int Cavuart::convert_baud(UartBaudRate baud) {
        int value = 115200;
        switch(baud) {
            case UartBaudRate::BAUD_1200 :
            {
                value = 1200;
            }
            case UartBaudRate::BAUD_2400 :
            {
                value = 2400;
            }
            case UartBaudRate::BAUD_4800 :
            {
                value = 4800;
            }
            case UartBaudRate::BAUD_9600 :
            {
                value = 9600;
            }
            case UartBaudRate::BAUD_19200 :
            {
                value = 19200;
            }
            case UartBaudRate::BAUD_38400 :
            {
                value = 38400;
            }
            case UartBaudRate::BAUD_57600 :
            {
                value = 57600;
            }
            case UartBaudRate::BAUD_115200 :
            {
                value = 115200;
            }
            case UartBaudRate::BAUD_230400 :
            {
                value = 230400;
            }
            case UartBaudRate::BAUD_460800 :
            {
                value = 460800;
            }
            case UartBaudRate::BAUD_921600 :
            {
                value = 921600;
            }
            default:
            {
            };
        }
        return value;
    };

    int Cavuart::convert_stopbits(UartStopBits stopbits) {
        int value = 1;
        switch(stopbits) {
            case UartStopBits::ONE :
            {
                value = 1;
            }
            case UartStopBits::ONE_AND_HALF :
            {
                value = 2;
            }
            case UartStopBits::TWO :
            {
                value = 3;
            }
            default:
            {
            };
        }
        return value;
    };

    flowcntrl_e Cavuart::convert_flow(UartHardwareFlowControl flow) {
        flowcntrl_e value = FC_NONE;
        switch(flow) {
            case UartHardwareFlowControl::NONE :
            {
                value = FC_NONE;
            }
            case UartHardwareFlowControl::RTS_CTS :
            {
                value = FC_RTSCTS;
            }
            case UartHardwareFlowControl::DTR_DSR :
            {
                value = FC_OTHER;
            }
            case UartHardwareFlowControl::XON_XOFF :
            {
                value = FC_XONXOFF;
            }
            default:
            {
            };
        }
        return value;
    };

    parity_e Cavuart::convert_parity(UartParity parity) {
        parity_e value = P_NONE;
        switch(parity) {
            case UartParity::NONE :
            {
                value = P_NONE;
            }
            case UartParity::EVEN :
            {
                value = P_EVEN;
            }
            case UartParity::ODD :
            {
                value = P_ODD;
            }
            default:
            {
            };
        }
        return value;
    };

    Return<Status> Cavuart::open_port(const hidl_string& name) {
        (void)name;
        Return<Status> ret = Status::NO_ERROR;
        if( tty_fd > 0 ) {
            ioctl(tty_fd, USERIAL_OP_CLK_OFF);
            close(tty_fd);
            tty_fd = -1;
        }
        tty_fd = open(name.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);
        if ( tty_fd > 0 ) {
            ioctl(tty_fd, USERIAL_OP_CLK_ON);
            keep_run = true;
            
            listenner = new std::thread([&](){
                struct pollfd fds[1];
                int ret;

                fds[0].fd = tty_fd;
                fds[0].events = POLLIN;
                while(keep_run) {
                    ret = poll(fds, 1, TIMEOUT);
                    if (ret < 0) {
                        // FAILED. MUST exist
                        ALOGE("POLL FAILED, force stop");
                        keep_run = false;
                        break;
                    } else if (ret == 0) {
                        // TIMEOUT.
                    } else {
                        char buffer[BUFFER_SIZE];
                        ssize_t bytes_read;
                        // Read all available data
                        while ((bytes_read = read(tty_fd, buffer, sizeof(buffer))) > 0) {
                            ALOGI("Read %i bytes", bytes_read);
                            std::string __str(buffer, bytes_read);

                            if(__callback!=nullptr) {
                                __callback->onDataReceived(hidl_vec<uint8_t>(__str.begin(), __str.end()));
                            }
                        }
                    }
                }
                ALOGI("Stop listen");
            });
        } else {
            ALOGE("Device busy");
            ret = Status::UNKNOWN_ERROR;
        }

        return ret;
    };

    Return<Status> Cavuart::close_port() {
        keep_run = false;
        listenner->join();
        delete listenner;

        if ( tty_fd > 0 ) {
            ioctl(tty_fd, USERIAL_OP_CLK_OFF);
            close(tty_fd);
            tty_fd = -1;
        } else {
            return Status::UNKNOWN_ERROR;
        }
        return Status::NO_ERROR;
    };

    Return<Status> Cavuart::transmit(const hidl_vec<uint8_t>& data) {
        (void)data;
        int sz = data.size(), n;
        char * tx_buf = 0;

        if ( tty_fd > 0 ) {
            tx_buf = new char[sz];
            std::copy(data.begin(), data.end(), tx_buf);
            ALOGI("transmit len=%i str=%s", sz, tx_buf);

            do {
                n = write(tty_fd, tx_buf, sz);
            } while ( n < 0 && errno == EINTR );

            delete tx_buf;
        } else {
            return Status::UNKNOWN_ERROR;
        }
        return Status::NO_ERROR;
    };

    Return<void> Cavuart::receive(int32_t length, receive_cb _hidl_cb) {
        (void)length;
        (void)_hidl_cb;
        return Void();
    };

    Return<void> Cavuart::registerCallback(const sp<IUartCallback>& callback) {
        (void)callback;
        __callback = callback;
        return Void();
    };

    Return<Status> Cavuart::configure(const UartConfig& config) {
        (void)config;
        int ret;
        int baud = 115200;
        enum flowcntrl_e flow = FC_NONE;
        enum parity_e parity = P_NONE;
        int databits = 8;
        int stopbits = 1;
        int noreset = 0;
        
        if ( tty_fd <= 0 ) {
            return Status::UNKNOWN_ERROR;
        }
        
        baud = convert_baud(config.baudRate);
        stopbits = convert_stopbits(config.stopBits);
        parity = convert_parity(config.parity);
        flow = convert_flow(config.hardwareFlowControl);

        ret = term_set(tty_fd,
                0,              /* raw mode. */
                baud,           /* baud rate. */
                parity,         /* parity. */
                databits,       /* data bits. */
                stopbits,       /* stop bits. */
                flow,           /* flow control. */
                1,              /* local or modem */
                !noreset);      /* hup-on-close. */
        return Status::NO_ERROR;
    };

}  // implementation
}  // namespace V1_0
}  // namespace uart
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor
