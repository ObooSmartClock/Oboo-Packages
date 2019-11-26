#if !defined(OBOO_RUNTIME_SERIAL_PORT_H_INCLUDED)
#define OBOO_RUNTIME_SERIAL_PORT_H_INCLUDED

#include "config.h"

#define 	DEFAULT_BAUDRATE		9600

int     initSerialPort        (char* portname, int baudrate);
void    destroySerialPort     ();

int     writeSerialPort     (char* msg);
int     writeCharSerialPort (unsigned char c);

int     readSerialPort      (char *msg);


#endif /* OBOO_RUNTIME_SERIAL_PORT_H_INCLUDED */
