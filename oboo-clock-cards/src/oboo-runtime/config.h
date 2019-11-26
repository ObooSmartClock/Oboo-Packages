#if !defined(OBOO_RUNTIME_CONFIG_H_INCLUDED)
#define OBOO_RUNTIME_CONFIG_H_INCLUDED


#define ORT_MQTT_ENABLED            1
#define ORT_SERIAL_PORT_ENABLED     1

#define MAX_NUMBER_OF_THREADS     5

#define MQ_HOST                     "localhost"
#define MQ_HOST_PORT                1883

#define MQ_REMOTE_HOST              "axkzq7gf6ury0.iot.us-east-1.amazonaws.com"
#define MQ_REMOTE_HOST_PORT         8883

#define MQ_CARD_TOPIC               "/card"

#define MQ_NUM_SUB_TOPICS          3
#define MQ_CARD_RESPONSE_TOPIC     "/cardResponse"
#define MQ_BUTTON_INPUT_TOPIC      "/button"
#define MQ_GESTURE_INPUT_TOPIC     "/gesture"

#define MQ_SET_BUTTON_COLOR_TOPIC   "/set"
#define MQ_SET_NIGHTLIGHT_TOPIC     MQ_SET_BUTTON_COLOR_TOPIC

#define MQ_SERCURE_CA_FILE          "/etc/certs/RootCA.pem"
#define MQ_SERCURE_CERT_FILE        "/etc/certs/certificate.pem.crt"
#define MQ_SERCURE_KEY_FILE         "/etc/certs/private.pem.key"

#define JS_EXIT_CONDITION						-1



#endif /* OBOO_RUNTIME_CONFIG_H_INCLUDED */
