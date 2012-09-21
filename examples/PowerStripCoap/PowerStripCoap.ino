/**
 *
 */

//Include Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <coap.h>
#include <RelaySens.h>

#include "PowerStrip.h"

Coap coap;
PowerStrip powerstrip;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(150, 140, 5, 64);
unsigned int localPort = 5683;      // local port to listen on

EthernetUDP EthUDP;

//Runs only once
void setup()
{
	DBG(Serial.begin(115200);
		Serial.println("");
		Serial.println("Init...");
	)

	pinMode(9, OUTPUT);

	Ethernet.begin(mac,ip);
	EthUDP.begin(localPort);

	coap.init(&Ethernet, &EthUDP);
	powerstrip.init(&coap);
	digitalWrite(9, HIGH);
	// resource id 0 is reserved for built in resource-discovery
	// init test resource, with resource id 1
	// inside init you must register a callback function

	DBG(Serial.println("Init done");)
}

void loop()
{
	// nothing else should be done here. CoAP service is running
	// if there is a request for your resource, your callback function will be triggered
	coap.handler();
}

