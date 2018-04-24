#ifndef _MQTTNETWORK_H_
#define _MQTTNETWORK_H_

#include "NetworkInterface.h"

/**
 * Lock this global mutex before any publish() calls.
 */
extern Mutex mqttMtx;

/**
 * @brief      MQTT Subscribe Thread Forwarding Table
 */
enum {
	FWD_TO_PRINT_THR = 0,
	FWD_TO_LED_THR   = 1,
	FWD_TO_MOVE_THR = 2,
	// FWD_TO_TEMP_THR  = 2
}; 

/**
 * Move thread task types
 */
enum move_command{
	MOVE_LEFT 			= 'a',
	MOVE_FORWARD_LEFT	= 'q',
	MOVE_FORWARD 		= 'w',
	MOVE_FORWARD_RIGHT	= 'e',
	MOVE_RIGHT 			= 'd',
	MOVE_BACK_RIGHT		= 'c',
	MOVE_BACK 			= 's',
	MOVE_BACK_LEFT		= 'z',
};
typedef char move_command;

/**
 * LED thread task types
 */
enum {
	LED_THR_PUBLISH_MSG,
	LED_ON_ONE_SEC,
	LED_BLINK_FAST
};

/**
 * Print thread task types
 */
enum {
	PRINT_MSG_TYPE_0,
	PRINT_MSG_TYPE_1
};

/**
 * @brief      ESP8266 and TCPSocket Wrapper for MQTTClient.h
 */
class MQTTNetwork {
public:
	MQTTNetwork(NetworkInterface* aNetwork) : network(aNetwork) {
		socket = new TCPSocket();
	}

	~MQTTNetwork() {
		delete socket;
	}

	int read(unsigned char* buffer, int len, int timeout) {
		socket->set_timeout(timeout);
		int ret = socket->recv(buffer, len);
		if (NSAPI_ERROR_WOULD_BLOCK == ret)
			return 0;
		else
			return ret;
	}

	int write(unsigned char* buffer, int len, int timeout) {
		return socket->send(buffer, len);
	}

	int connect(const char* hostname, int port) {
		socket->open(network);
		return socket->connect(hostname, port);
	}

	int disconnect() {
		return socket->close();
	}

private:
	NetworkInterface* network;
	TCPSocket* socket;
};

#endif /* _MQTTNETWORK_H_ */