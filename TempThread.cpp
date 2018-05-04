/**
 * Copyright (c) 2017, Autonomous Networks Research Group. All rights reserved.
 * Developed by:
 * Autonomous Networks Research Group (ANRG)
 * University of Southern California
 * http://anrg.usc.edu/
 *
 * Contributors:
 * Jason A. Tran <jasontra@usc.edu>
 * Bhaskar Krishnamachari <bkrishna@usc.edu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *     this list of conditions and the following disclaimers in the 
 *     documentation and/or other materials provided with the distribution.
 * - Neither the names of Autonomous Networks Research Group, nor University of 
 *     Southern California, nor the names of its contributors may be used to 
 *     endorse or promote products derived from this Software without specific 
 *     prior written permission.
 * - A citation to the Autonomous Networks Research Group must be included in 
 *     any publications benefiting from the use of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH 
 * THE SOFTWARE.
 */

/**
 * @file       TempThread.cpp
 * @brief      Implementation of thread that handles Temperature requests.
 *
 * @author     Jason Tran <jasontra@usc.edu>
 * @author     Bhaskar Krishnachari <bkrishna@usc.edu>
 */

#include "TempThread.h"
#include "MQTTmbed.h"
#include "MQTTNetwork.h"

#include "MQTTClient.h"

Mail<MailMsg, TEMPTHREAD_MAILBOX_SIZE> TempMailbox;

// Necessary to check how to attach the Temperature and Humidity Sensor
static DHT sensor(p18,SEN11301P); // Use the SEN11301P sensor

static const char *topic = "m3pi-mqtt-ee250/temp-thread";

void tempThread(void *args) 
{
	MQTT::Client<MQTTNetwork, Countdown> *client = (MQTT::Client<MQTTNetwork, Countdown> *)args;
	MailMsg *msg;
	MQTT::Message message;
	osEvent evt;
	char pub_buf[16];


	while(1) {

		evt = TempMailbox.get();

		if(evt.status == osEventMail) {
			msg = (MailMsg *)evt.value.p;

			/* the second byte in the message denotes the action type */
			switch (msg->content[1]) {
				case TEMP_THR_PUBLISH_MSG:
					printf("TempThread: received command to publish to topic"
						   "m3pi-mqtt-example/temp-thread\n");
					// Block until a appropriate sensor value is given
					while (sensor.readData() != 0)	
						sprintf(pub_buf, "%4.2f,%4.2f", sensor.ReadTemperature(CELCIUS), sensor.ReadHumidity());
					message.qos = MQTT::QOS0;
					message.retained = false;
					message.dup = false;
					message.payload = (void*)pub_buf;
					message.payloadlen = 9; //MQTTclient.h takes care of adding null char?
					/* Lock the global MQTT mutex before publishing */
					mqttMtx.lock();
					client->publish(topic, message);
					mqttMtx.unlock();
					break;
				default:
					printf("TempThread: invalid message\n");
					break;
			}            

			TempMailbox.free(msg);
		}
	} /* while */

	/* this should never be reached */
}

Mail<MailMsg, TEMPTHREAD_MAILBOX_SIZE> *getTempThreadMailbox() 
{
	return &TempMailbox;
}


