#include "serialPort.h"

# if ORT_SERIAL_PORT_ENABLED == 1

// includes
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "runtime.h"

// defines
// TODO: setup ifndef's and define BAUD_RATE
#define FD_DEFAULT      -1

// global vars
int serialFd = FD_DEFAULT;

// local functions
int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    /* set at least 1 byte and enable inter-byte timeout - will lead to a blocking call until at least 1 byte is read */
    // tty.c_cc[VMIN] = 1;
    // tty.c_cc[VTIME] = 1;
    /* set polling mode - reading all available bytes */
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    // 0 - non-blocking read, 1 - blocking read
    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

void set_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        printf("ERROR: %d from tggetattr", errno);
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
        printf("ERROR: %d setting term attributes", errno);
}
#endif  // ORT_SERIAL_PORT_ENABLED

int initSerialPort(char* portname, int baudrate){

# if ORT_SERIAL_PORT_ENABLED == 1
    speed_t baud;
    printf("Attempting to initialize serial port '%s' with baudrate '%d'...\n", portname, baudrate);
    serialFd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (serialFd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

    // convert baudrate int
    switch (baudrate) {
      case 115200:
        baud = B115200;
        break;
      case 9600:
      default:
        baud = B9600;
        break;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(serialFd, baud);
    // set_blocking (serialFd, 0);                // set no blocking - read function returns however many characters are available without waiting
    printf("... success!\n");

#endif  // ORT_SERIAL_PORT_ENABLED
    return 0;
}

void destroySerialPort() {
# if ORT_SERIAL_PORT_ENABLED == 1
    close(serialFd);
#endif  // ORT_SERIAL_PORT_ENABLED
}

int writeSerialPort(char* msg) {
    int wlen;
    int expLen = strlen(msg);

    if (serialFd != FD_DEFAULT) {
        // printf("  writeSerialPort writing message of length %d\n", expLen);
        wlen = write(serialFd, msg, expLen);
        if (wlen != expLen) {
            printf("Error from write: %d, %d\n", wlen, errno);
            return -1;
        }
        // printf("  pause...");
        // onion.io: tcdrain occasionally causes seg fault...
        // tcdrain(serialFd);    /* delay for output */
        // onion.io: manual wait based on length of message (9600 bits per second)
        usleep(expLen*8*104);   // 9600 bits per second = 104.16 us/bit,    8 bits per character
        // printf(" complete\n");
    }
    else {
        printf("ERROR: no file descriptor for serial port\n");
        return -1;
    }

    return 0;
}

int writeCharSerialPort(unsigned char c) {
    int wlen;
    int expLen = 1;

    if (serialFd != FD_DEFAULT) {
        wlen = write(serialFd, &c, expLen);
        if (wlen != expLen) {
            printf("Error from write: %d, %d\n", wlen, errno);
            return -1;
        }
        // onion.io: manual wait based on length of message (9600 bits per second)
        usleep(expLen*8*104);   // 9600 bits per second = 104.16 us/bit,    8 bits per character
    }
    else {
        printf("ERROR: no file descriptor for serial port\n");
        return -1;
    }

    return 0;
}

int readSerialPort(char *msg) {
    unsigned char buf[MAX_CHAR_LEN];
    int rdlen;

    rdlen = read(serialFd, buf, sizeof(buf) - 1);
    if (rdlen > 0) {
        buf[rdlen] = 0;
        printf("Read %d: \"%s\"\n", rdlen, buf);
        strncpy(msg, buf, rdlen);
    } else if (rdlen < 0) {
        printf("Error from read: %d: %s\n", rdlen, strerror(errno));
    }

    return rdlen;
}
