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

    uint32_t Cavuart::convert_baud(UartBaudRate baud) {
        uint32_t value = B115200;
        switch(baud) {
            case UartBaudRate::BAUD_1200 :
            {
                value = B1200;
                break;
            }
            case UartBaudRate::BAUD_2400 :
            {
                value = B2400;
                break;
            }
            case UartBaudRate::BAUD_4800 :
            {
                value = B4800;
                break;
            }
            case UartBaudRate::BAUD_9600 :
            {
                value = B9600;
                break;
            }
            case UartBaudRate::BAUD_19200 :
            {
                value = B19200;
                break;
            }
            case UartBaudRate::BAUD_38400 :
            {
                value = B38400;
                break;
            }
            case UartBaudRate::BAUD_57600 :
            {
                value = B57600;
                break;
            }
            case UartBaudRate::BAUD_115200 :
            {
                value = B115200;
                break;
            }
            case UartBaudRate::BAUD_230400 :
            {
                value = B230400;
                break;
            }
            case UartBaudRate::BAUD_460800 :
            {
                value = B460800;
                break;
            }
            case UartBaudRate::BAUD_921600 :
            {
                value = B921600;
                break;
            }
            case UartBaudRate::BAUD_1000000 :
            {
                value = B1000000;
                break;
            }
            case UartBaudRate::BAUD_1152000 :
            {
                value = B1152000;
                break;
            }
            case UartBaudRate::BAUD_1500000 :
            {
                value = B1500000;
                break;
            }

            case UartBaudRate::BAUD_2000000 :
            {
                value = B2000000;
                break;
            }
            case UartBaudRate::BAUD_2500000 :
            {
                value = B2500000;
                break;
            }
            case UartBaudRate::BAUD_3000000 :
            {
                value = B3000000;
                break;
            }

            case UartBaudRate::BAUD_3500000 :
            {
                value = B3500000;
                break;
            }
            case UartBaudRate::BAUD_4000000 :
            {
                value = B4000000;
                break;
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
                value = 0;
            }
            case UartStopBits::ONE_AND_HALF :
            {
                value = 2;
            }
            case UartStopBits::TWO :
            {
                value = 2;
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

    uint16_t Cavuart::convert_parity(UartParity parity) {
        uint16_t value = P_NONE;
        switch(parity) {
            case UartParity::NONE :
            {
                value = 0;
            }
            case UartParity::EVEN :
            {
                value = PARENB;
            }
            case UartParity::ODD :
            {
                value = (PARENB | PARODD);
            }
            default:
            {
            };
        }
        return value;
    };

    Cavuart::Cavuart(void) {
        listenner = nullptr;
    }

    Return<Status> Cavuart::open_port(const hidl_string& name) {
        (void)name;
        Return<Status> ret = Status::NO_ERROR;

        ALOGI("open_port");

        if( tty_fd > 0 ) {
            close_port();
        }

        tty_fd = open(name.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);
        if ( tty_fd > 0 ) {
            ioctl(tty_fd, USERIAL_OP_CLK_ON);
            keep_run = true;
            
            if ( nullptr == listenner) {
                listenner = new std::thread([&](){
                    struct pollfd fds[1];
                    int ret;
                    char buffer[BUFFER_SIZE];
                    ssize_t bytes_read=0;
                    ssize_t total_bytes_read=0;

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
                            // Read all available data
                            do {
                                bytes_read = read(tty_fd, (&buffer[total_bytes_read]), (sizeof(buffer)-total_bytes_read));
                                if(bytes_read>0) {
                                    ALOGI("[1] total_bytes_read %zi bytes", total_bytes_read);
                                    ALOGI("Read %zi bytes", bytes_read);
                                    total_bytes_read = bytes_read + total_bytes_read;
                                    ALOGI("[2] total_bytes_read %zi bytes", total_bytes_read);
                                }
                            } while (bytes_read>0);

                            if( total_bytes_read>0 && __callback!=nullptr) {
                                std::string __str(buffer, total_bytes_read);
                                ALOGI("total_bytes_read %zi bytes", total_bytes_read);
                                __callback->onDataReceived(hidl_vec<uint8_t>(__str.begin(), __str.end()));
                                total_bytes_read=0;
                            }
                        }
                    }
                    ALOGI("Stop listen");
                });
            };
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
        listenner = nullptr;

        ALOGI("close_port");

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
            ALOGI("transmit len=%i", sz);

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
        int baud = 115200;
        enum flowcntrl_e flow = FC_NONE;
        uint16_t parity = P_NONE;
        int stopbits = 1;
        struct termios termios;

        ALOGI("Run configure");
        if ( tty_fd <= 0 ) {
            return Status::UNKNOWN_ERROR;
        }
        baud = convert_baud(config.baudRate);
        stopbits = convert_stopbits(config.stopBits);
        parity = convert_parity(config.parity);
        flow = convert_flow(config.hardwareFlowControl);
        ALOGI("config baud=%i", baud);

        tcflush(tty_fd, TCIOFLUSH);
        tcgetattr(tty_fd, &termios);
        cfmakeraw(&termios);

        /* Set UART Control Modes */
        termios.c_cflag &= ~(CRTSCTS);
        termios.c_cflag |= stopbits;
        termios.c_cflag |= CLOCAL;

        termios.c_cc[VTIME]    = 0;   /* inter-character timer 10ms */
        termios.c_cc[VMIN]     = 1;    /* blocking read until 1 chars received */

        if ( flow != FC_NONE ) {
            /* Enable CTS/RTS flow control */
            termios.c_cflag |= (CRTSCTS); 
        }
        tcsetattr(tty_fd, TCSANOW, &termios);
        tcflush(tty_fd, TCIOFLUSH);
        tcflush(tty_fd, TCIOFLUSH);

        cfsetospeed(&termios, baud);
        cfsetispeed(&termios, baud);
        tcsetattr(tty_fd, TCSANOW, &termios);
        return Status::NO_ERROR;
    };

}  // implementation
}  // namespace V1_0
}  // namespace uart
}  // namespace hardware
}  // namespace cavli
}  // namespace vendor
