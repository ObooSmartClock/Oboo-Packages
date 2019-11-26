#if !defined(OBOO_RUNTIME_MQTT_H_INCLUDED)
#define OBOO_RUNTIME_MQTT_H_INCLUDED

#include "config.h"

#define CLIENT_ID_LEN       10
#define MAX_PAYLOAD_SIZE    8192

// types
typedef enum {
    MQ_LOCAL_CLIENT = 0,
    MQ_REMOTE_CLIENT = 1,
    MQ_NUM_CLIENTS = 2
} mq_client_type;



// function definitions
// TODO: move these two functions to a utils file
char*   generateRandomString    (int length);
char*   generateRandomNumber    (int length);
int     generateRandomInteger   (int n);

void    initMessageQueue        ();
void    destroyMessageQueue     ();

int     connectMessageQueue         (int clientType, char* host, int port, char* clientId, char* willTopic, char* willPayload);
int     connectMessageQueueSecure   (int clientType, char* host, int port, char* clientId, char* willTopic, char* willPayload);


int     sendMessage                 (int clientType, char* topic, char* payload);
int     messageQueueSubscribe       (int clientType, char* topic, int *id);


#endif /* OBOO_RUNTIME_MQTT_H_INCLUDED */

/*
connection bridge-to-aws
address axkzq7gf6ury0.iot.us-east-1.amazonaws.com:8883
bridge_cafile /etc/awsiot/RootCA.pem
cleansession true
try_private false
bridge_attempt_unsubscribe false
bridge_insecure false
notifications false
bridge_certfile /etc/awsiot/Omega-7CCB-certificate.pem.crt
bridge_keyfile /etc/awsiot/Omega-7CCB-private.pem.key
remote_clientid Omega-7CCB
*/
