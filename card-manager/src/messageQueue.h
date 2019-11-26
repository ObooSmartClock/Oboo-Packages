#ifndef OBOO_MESSAGE_QUEUE_H
#define OBOO_MESSAGE_QUEUE_H

#include "config.h"


int     initMessageQueue        (char* host, int port);
void    destroyMessageQueue     ();

int     sendMessage             (char* topic, char* payload);
int     messageQueueSubscribe   (char* topic);


#endif  // OBOO_MESSAGE_QUEUE_H