/**
 * XbeeRadio Arduino library by Vasilis (tzikis) Georgitzikis (tzikis.com).
 * XbeeRadio uses another awesome Open Source Arduino library, XBee-Arduino.
 *
 * XbeeRadio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *
 * This is an example of a default receiver and transmiter.
 * Upon receiving a packet, we send it back to the sender.
 *
 */


//Include Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetCoap.h>
//#include <SoftwareSerial.h>
#include <coap.h>
#include "App.h"

#ifdef DEBUG
// software serial port
SoftwareSerial mySerial(2, 3);
#endif

// coap object
Coap coap;
// resources
//resource_t resources[CONF_MAX_RESOURCES];
// your app
App testApp;
// resource buffer
//uint8_t buf[CONF_MAX_PAYLOAD_LEN];
//char largeBuf[CONF_LARGE_BUF_LEN];
//Create the XbeeRadio object we'll be using

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(150, 140, 5, 120);
IPAddress bc(150, 140, 5, 255);
unsigned int localPort = 5683;      // local port to listen on

EthernetCoap Ethcoap;

//Runs only once
void setup()
{
	//pinMode(10, OUTPUT);
	pinMode(12, OUTPUT);

	Ethernet.begin(mac,ip);
	Ethcoap.begin(localPort);
	Ethcoap.setBroadCast(bc);

#ifdef DEBUG
	mySerial.begin(9600);
	mySerial.println("INIT...");
	coap.init(&timer, &mySerial, &Ethcoap, resources, buf, largeBuf);
	testApp.init(resources, 1, largeBuf);
	mySerial.println("INIT DONE");
#else
	coap.init(&Ethernet, &Ethcoap);
	testApp.init(&coap);
#endif
	digitalWrite(12, HIGH);
	// resource id 0 is reserved for built in resource-discovery
	// init test resource, with resource id 1
	// inside init you must register a callback function
}

void loop()
{
	// nothing else should be done here. CoAP service is running
	// if there is a request for your resource, your callback function will be triggered
	coap.handler();
}

