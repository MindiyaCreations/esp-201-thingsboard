#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <libconfig.h>
#include <string.h>
#include "ini.h"

typedef struct
{
    const char* THINGSBOARD_HOST;
    const char* BROKER_HOST;
    const char* THINGSBOARD_CLIENT;
    const char* BROKER_CLIENT;
    int THINGSBOARD_PORT;
    int BROKER_PORT;
    int DEBUG;
} configuration;
configuration config;

static int handler(void* user, const char* section, const char* name,const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("GLOBAL", "DEBUG")) {
        pconfig->DEBUG = atoi(value);
    } else if (MATCH("BROKER", "HOST")) {
        pconfig->BROKER_HOST = strdup(value);
    } else if (MATCH("BROKER", "CLIENT")) {
        pconfig->BROKER_CLIENT = strdup(value);
    } else if (MATCH("BROKER", "PORT")) {
        pconfig->BROKER_PORT = atoi(value);
    }  else if (MATCH("THINGSBOARD", "HOST")) {
        pconfig->THINGSBOARD_HOST = strdup(value);
    }  else if (MATCH("THINGSBOARD", "CLIENT")) {
        pconfig->THINGSBOARD_CLIENT = strdup(value);
    }  else if (MATCH("THINGSBOARD", "PORT")) {
        pconfig->THINGSBOARD_PORT = atoi(value);
    }  else {
        return 0;
    }
    return 1;
}

struct mosquitto *broker;
struct mosquitto *thingsboard;

void on_connect_broker(struct mosquitto *mosq, void *obj, int rc) {
	printf("Connected Broker\n");
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "#", 0);
}

void on_disconnect_broker(struct mosquitto *mosq, void *obj, int rc) {
	printf("Disconnected Broker\n");
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
}

void on_message_broker(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	if(config.DEBUG == 1){
		printf("Broker %s: %s\n", msg->topic, (char *) msg->payload);
	}
	mosquitto_publish(thingsboard, NULL, msg->topic, msg->payloadlen, msg->payload, msg->qos, msg->retain);
}

void on_connect_thingsboard(struct mosquitto *mosq, void *obj, int rc) {
	printf("Connected Thingsboard\n");
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "#", 0);
}

void on_disconnect_thingsboard(struct mosquitto *mosq, void *obj, int rc) {
	printf("Disconnected Thingsboard\n");
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
}

void on_message_thingsboard(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	if(config.DEBUG == 1){
		printf("Thingsboard %s: %s\n", msg->topic, (char *) msg->payload);
	}
	mosquitto_publish(broker, NULL, msg->topic, msg->payloadlen, msg->payload, msg->qos, msg->retain);
}

int main() {
    if (ini_parse("gateway.conf", handler, &config) < 0) {
        printf("Can't load 'gateway.conf'\n");
        return 1;
    }
    if(config.DEBUG == 1){
		printf("BROKER %s %s %d\n",config.BROKER_HOST,config.BROKER_CLIENT,config.BROKER_PORT);
        printf("THINGSBOARD %s %s %d\n",config.THINGSBOARD_HOST,config.THINGSBOARD_CLIENT,config.THINGSBOARD_PORT);
	}

	int rc, id=12;
	mosquitto_lib_init();

	//Start Broker MQTT connection
	broker = mosquitto_new(config.BROKER_CLIENT, true, &id);
	mosquitto_connect_callback_set(broker, on_connect_broker);
	mosquitto_disconnect_callback_set(broker, on_disconnect_broker);
	mosquitto_message_callback_set(broker, on_message_broker);
	rc = mosquitto_connect(broker, config.BROKER_HOST, config.BROKER_PORT, 10);
	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}
	mosquitto_loop_start(broker);

	//Start Thingsboard MQTT connection
	thingsboard = mosquitto_new(config.THINGSBOARD_CLIENT, true, &id);
	mosquitto_connect_callback_set(thingsboard, on_connect_thingsboard);
	mosquitto_disconnect_callback_set(thingsboard, on_disconnect_thingsboard);
	mosquitto_message_callback_set(thingsboard, on_message_thingsboard);
	rc = mosquitto_connect(thingsboard, config.THINGSBOARD_HOST, config.THINGSBOARD_PORT, 10);
	if(rc) {
		printf("Could not connect to Thingsboard with return code %d\n", rc);
		return -1;
	}
	mosquitto_loop_start(thingsboard);

	while(true);

	//Closing Broker MQTT connection
	mosquitto_loop_stop(broker, true);
	mosquitto_disconnect(broker);
	mosquitto_destroy(broker);

	//Closing Thingsboard MQTT connection
	mosquitto_loop_stop(thingsboard, true);
	mosquitto_disconnect(thingsboard);
	mosquitto_destroy(thingsboard);


	mosquitto_lib_cleanup();

	return 0;
}