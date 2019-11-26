// includes
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mosquitto.h"

#include "runtime.h"
#include "messageQueue.h"

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

struct mosquitto *mosqPtr;
int bConnected;


// local functions
void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	int i;
	char* payloadptr;
	char payload[1024];
    duk_context *dukCtx = (duk_context*)userdata;
    
    printf("Message arrived to topic: %s, message is %d chars long\n", (char*)message->topic, message->payloadlen);
    printf("   message: '%s'\n", (char*)message->payload);
    
    handleMessage((char*)message->topic, (char*)message->payload);
}

void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result){
		/* Subscribe to required topics on successful connect. */
		mosquitto_subscribe(mosq, NULL, MQ_CARD_TOPIC, MQTT_QOS);
		mosquitto_subscribe(mosq, NULL, MQ_STATUS_TOPIC, MQTT_QOS);
		mosquitto_subscribe(mosq, NULL, MQ_NOTIFICATION_TOPIC, MQTT_QOS);
	}else{
		fprintf(stderr, "Connect failed\n");
	}
}

void mqtt_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;

	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void mqtt_disconnect_callback(struct mosquitto *mosq, void *userdata, int rc) {
	printf("Message Queue disconnected! (%d)\n", rc);
	// TODO: potentially attempt to reconnect here?
}

int messageQueueConnect(char* host, int port) {
	printf("initializing message queue\n");
    bConnected = 0;
    
    // instantiate new client (with a unique client id)
    mosqPtr = mosquitto_new(NULL, true, NULL);
    // onion.io: note that userdata is not passed along for now
    if(!mosqPtr){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	
	// set callback functions
	mosquitto_connect_callback_set(mosqPtr, mqtt_connect_callback);
	mosquitto_message_callback_set(mosqPtr, mqtt_message_callback);
	mosquitto_subscribe_callback_set(mosqPtr, mqtt_subscribe_callback);
	mosquitto_disconnect_callback_set(mosqPtr, mqtt_disconnect_callback);
	
	// start the connection
	if(mosquitto_connect(mosqPtr, host, port, MQTT_KEEPALIVE)){
		fprintf(stderr, "MQTT unable to connect.\n");
		mosquitto_destroy(mosqPtr);
		return 1;
	}
	
	// start the loop
	int loop = mosquitto_loop_start(mosqPtr);
	if(loop != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Unable to start loop: %i\n", loop);
		mosquitto_destroy(mosqPtr);
		return 1;
	}
	
	return 0;
}

// API functions
int initMessageQueue(char* host, int port) {
	int count = 0;
	int status = 1;
	
	// initialize the library
	mosquitto_lib_init();
	
	while (count < MQTT_MAX_CONNECT_TRIES) {
		if (messageQueueConnect(host, port) == 0) {
			printf("message queue initialized (%d)\n", count);
			status = 0;
			break;	
		}
		count++;
		sleep(MQTT_CONNECT_WAIT);
	}
	
	return status;	
}

void destroyMessageQueue () {
    /* mqtt release */
    mosquitto_destroy(mosqPtr);
    mosquitto_lib_cleanup();
}

int sendMessage (char* topic, char* payload) {
	int status = 0;

	status = mosquitto_publish(	mosqPtr,
								NULL,
								topic,
								strlen(payload),
								payload,
								MQTT_QOS,
								false
							);

	return status;
}

int messageQueueSubscribe (char* topic) {
	int status = -1;

	printf("Subscribe to topic '%s' ", topic);
	status =  mosquitto_subscribe(	mosqPtr,
									NULL,
									topic,
									MQTT_QOS
								);
	if (status == MOSQ_ERR_SUCCESS) {
		printf("successful\n");
	} else if (status == MOSQ_ERR_NO_CONN) {
		printf("not successful, not connected to a broker\n");
	}
	
	return status;
}