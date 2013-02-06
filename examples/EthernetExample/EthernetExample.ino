/**
 *
 */
//Uncomment for DEBUG in Serial Port
#define DEBUG

//Ethernet Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
byte mac[] = {0xAA, 0xAD, 0xBA, 0xED, 0xFE, 0xED};
IPAddress ip(150, 140, 5, 122);
unsigned int localPort = 5683;      // local port to listen on
EthernetUDP EthUDP;

//Coap Library
#include <coap.h>
#include "GetSensor.h"
#include "PostSensor.h"
#include "AnalogSensor.h"
Coap coap;


//Runs only once
void setup()
{
	DBG(
            Serial.begin(115200);
            Serial.println("");
            Serial.println("Setup...");
            )

	pinMode(9, OUTPUT);

        //Start UDP server
	Ethernet.begin(mac,ip);
	EthUDP.begin(localPort);

        //Init Coap
	coap.init(&Ethernet, &EthUDP);

        CoapSensor * example_get_resource = new getSensor("getSens");
        CoapSensor * example_post_resource =new postSensor("postSens");
        CoapSensor * example_analog_resource = new analogSensor("analogSens",A0);
        //Add Resources
        coap.add_resource(example_post_resource);
        coap.add_resource(example_get_resource);
        coap.add_resource(example_analog_resource);
        
        
	digitalWrite(9, HIGH);

	DBG(
            Serial.println("Setup...Done!");
            )
}

void loop()
{
	// nothing else should be done here. CoAP service is running
	// if there is a request for your resource, your callback function will be triggered
	coap.handler();
}