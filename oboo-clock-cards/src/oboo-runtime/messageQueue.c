#include "messageQueue.h"

#if ORT_MQTT_ENABLED == 1
// includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mosquitto.h"
#include "runtime.h"

#define MQTT_HOST     		"localhost"
#define MQTT_PORT			1883
// TODO: add ifndef to these ones
#define MQTT_QOS         	0
#define MQTT_KEEPALIVE		60

#define MQTT_MAX_CONNECT_TRIES		10
#define MQTT_CONNECT_WAIT			5

// global vars
struct topicList {
	char**  list;
	int 	numElements;
} ;

struct mosquitto *mosqPtrs[MQ_NUM_CLIENTS];

// local functions
void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result){
		/* invoke js callback on successful connect. */
		// printf("Connect with userdata '%s'\n", (char*)userdata);
		invokeCallback((char*)userdata);
	}else{
		fprintf(stderr, "Connect failed\n");
	}
}

void mqtt_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;
	char buf[MAX_CHAR_LEN];

	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");

	// invoke callback if any
	sprintf(buf, "s%d", mid);
	invokeCallback(buf);
}

void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	int i;
	char payload[MAX_PAYLOAD_SIZE];

    printf("Message arrived to topic: %s, message is %d chars long\n", message->topic, message->payloadlen);

    // pass the topic and message to the runtime
    handleMessage((char*)message->topic,  (char*)message->payload);
}

void mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int rc) {
	printf("Message Queue disconnected - %s (%d)\n", mosquitto_strerror(rc), rc);
	// TODO: potentially attempt to reconnect here?
}

char *_generateRandomChars(int length, char charset[]) {
	char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * ((size_t)length +1));

        if (randomString) {
            for (int n = 0;n < length;n++) {
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[(size_t)length] = '\0';
        }
    }

    return randomString;
}

char *generateRandomString(int length) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    return _generateRandomChars(length, charset);
}

char *generateRandomNumber(int length) {
    static char charset[] = "0123456789";

    return _generateRandomChars(length, charset);
}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int generateRandomInteger(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    // assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    // assert (end > 0)
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}


int _setTls(int clientType, char *cafilePath, char* certfilePath, char* keyfilePath) {
	int status;
	if (cafilePath == NULL || certfilePath == NULL || keyfilePath == NULL) {
		status = mosquitto_tls_set(
					mosqPtrs[clientType],
 					(const char*)MQ_SERCURE_CA_FILE,	//const char *cafile,
 					NULL,	//const char *capath,	- note: can be NULL if cafile is defined
 					(const char*)MQ_SERCURE_CERT_FILE,	//const char *certfile,
 					(const char*)MQ_SERCURE_KEY_FILE,	//const char *keyfile,
 					NULL	//int (*pw_callback)(char *buf, int size, int rwflag, void *userdata)
 				);
	} else {
		status = mosquitto_tls_set(
					mosqPtrs[clientType],
 					(const char*)cafilePath,	//const char *cafile,
 					NULL,	//const char *capath,	- note: can be NULL if cafile is defined
 					(const char*)certfilePath,	//const char *certfile,
 					(const char*)keyfilePath,	//const char *keyfile,
 					NULL	//int (*pw_callback)(char *buf, int size, int rwflag, void *userdata)
 				);
	}

	if (status != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "ERROR:: TLS Setup failed: %s (%d)\n", mosquitto_strerror(status), status);
		return 1;
	}
	return 0;
}

int _connectMessageQueue(int clientType, char* host, int port, char* clientId, int bTls, char* willTopic, char* willPayload) {
	int status = 0;

	if (clientId == NULL) {
		clientId = generateRandomString(CLIENT_ID_LEN);
	}

	// instantiate new client (pass clientId as userdata)
    mosqPtrs[clientType] = mosquitto_new(clientId, true, clientId);
    if(!mosqPtrs[clientType]){
		fprintf(stderr, "ERROR: Out of memory - could not instantiate message queue client\n");
		return 1;
	}

	printf("connecting to message queue at %s:%d, clientId is '%s', ptr is %d\n", host, port, clientId, clientType);

	if (bTls) {
		_setTls(clientType, NULL, NULL, NULL);
	}

	// set callback functions
	mosquitto_connect_callback_set(mosqPtrs[clientType], mqtt_connect_callback);
	mosquitto_message_callback_set(mosqPtrs[clientType], mqtt_message_callback);
	mosquitto_subscribe_callback_set(mosqPtrs[clientType], mqtt_subscribe_callback);
	mosquitto_disconnect_callback_set(mosqPtrs[clientType], mqtt_disconnect_callback);

	// set will (if any)
	if (willTopic != NULL && willPayload != NULL) {
		status = mosquitto_will_set( mosqPtrs[clientType],
									willTopic,		// topic
									strlen(willPayload),				// payloadlen
									willPayload,	// pauload
									MQTT_QOS, // qos
									false // retain (boolean)
								);
		if (status != MOSQ_ERR_SUCCESS) {
			fprintf(stderr, "ERROR: message queue unable to set will: %s (%d)\n", mosquitto_strerror(status), status);
		}
	}

	// start the connection
	printf("Initializing mqtt connection to %s:%d\n", host, port);
	status = mosquitto_connect(mosqPtrs[clientType], host, port, MQTT_KEEPALIVE);
	if(status != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "ERROR: message queue unable to connect: %s (%d)\n", mosquitto_strerror(status), status);
		mosquitto_destroy(mosqPtrs[clientType]);
		return 1;
	}

	// start the loop
	status = mosquitto_loop_start(mosqPtrs[clientType]);
	if(status != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "ERROR: Unable to start message client loop: %s (%d)\n", mosquitto_strerror(status), status);
		mosquitto_destroy(mosqPtrs[clientType]);
		return 1;
	}

	return status;
}

int _connectMessageQueueSequence(int clientType, char* host, int port, char* clientId, int bTls, char* willTopic, char* willPayload) {
	int count = 0;
	int status = 1;

	while (count < MQTT_MAX_CONNECT_TRIES) {
		if (_connectMessageQueue(clientType, host, port, clientId, bTls, willTopic, willPayload) == 0) {
			printf("message queue initialized (%d)\n", count);
			status = 0;
			break;
		}
		count++;
		sleep(MQTT_CONNECT_WAIT);
	}

	return status;

}

#endif  // ORT_MQTT_ENABLED

// API functions
void initMessageQueue() {
#if ORT_MQTT_ENABLED == 1
	mosquitto_lib_init();
	for (int i = 0; i < MQ_NUM_CLIENTS; i++) {
		mosqPtrs[i] = NULL;
	}
#endif  // ORT_MQTT_ENABLED
}

void destroyMessageQueue () {
    /* mqtt release */
#if ORT_MQTT_ENABLED == 1
	for (int i = 0; i < MQ_NUM_CLIENTS; i++) {
		if (mosqPtrs[i] != NULL) {
	    	mosquitto_destroy(mosqPtrs[i]);
		}
	}

	mosquitto_lib_cleanup();
#endif  // ORT_MQTT_ENABLED
}


int connectMessageQueue(int clientType, char* host, int port, char* clientId, char* willTopic, char* willPayload) {
	int status = 0;
#if ORT_MQTT_ENABLED == 1
	status = _connectMessageQueueSequence(clientType, host, port, clientId, 0, willTopic, willPayload);
#endif  // ORT_MQTT_ENABLED

	return status;
}

int connectMessageQueueSecure(int clientType, char* host, int port, char* clientId, char* willTopic, char* willPayload) {
	int status = 0;
#if ORT_MQTT_ENABLED == 1

	if (host == NULL || strcmp(host,JS_NULL) == 0) {
		host = MQ_REMOTE_HOST;
	}
	status = _connectMessageQueueSequence(clientType, host, port, clientId, 1, willTopic, willPayload);
#endif  // ORT_MQTT_ENABLED

	return status;
}

int sendMessage (int clientType, char* topic, char* payload) {
	int status = 0;

#if ORT_MQTT_ENABLED == 1
	struct mosquitto *mosqPtr = mosqPtrs[clientType];
	if (mosqPtr != NULL) {
		status = mosquitto_publish(	mosqPtr,
									NULL,
									topic,
									strlen(payload),
									payload,
									MQTT_QOS,
									false
								);
	} else {
		fprintf(stderr, "Error: invalid pointer (%d) for publish.\n", clientType);
	}
#endif  // ORT_MQTT_ENABLED

	return status;
}

int messageQueueSubscribe (int clientType, char* topic, int* id) {
	int status = -1;

#if ORT_MQTT_ENABLED == 1
	struct mosquitto *mosqPtr = mosqPtrs[clientType];
	if (mosqPtr != NULL) {
		printf("Subscribe to topic '%s' ", topic);
		status =  mosquitto_subscribe(	mosqPtr,
										id,
										topic,
										MQTT_QOS
									);
		if (status == MOSQ_ERR_SUCCESS) {
			printf("successful\n");
		} else if (status == MOSQ_ERR_NO_CONN) {
			printf("not successful, not connected to a broker\n");
		}
	} else {
		fprintf(stderr, "Error: invalid pointer (%d) for subscrube.\n", clientType);
	}

#endif  // ORT_MQTT_ENABLED

	return status;
}
