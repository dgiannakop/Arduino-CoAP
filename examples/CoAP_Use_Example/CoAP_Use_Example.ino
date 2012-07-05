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
#include <XBee.h>
#include <XbeeRadio.h>
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
XBeeRadio xbee = XBeeRadio();
// create a reusable response object for responses we expect to handle
XBeeRadioResponse response = XBeeRadioResponse();
// create a reusable rx16 response object to get the address
Rx16Response rx = Rx16Response();

//Runs only once
void setup()
{
  //pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite( 12, LOW );
  digitalWrite( 2, HIGH);
  digitalWrite( 3, LOW );
  digitalWrite( 4, LOW );
  digitalWrite( 5, LOW );
  digitalWrite( 6, LOW );
  // comment out for debuging
  xbee.initialize_xbee_module();
  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin( 38400 );
  //Initialize our XBee module with the correct values (using the default channel, channel 12)h
  xbee.init(12);
  // set coap object for callback functions
  //Wrapper::setObj(coap);
  // init coap service 
#ifdef DEBUG
  mySerial.begin(9600);
  mySerial.println("INIT...");
  coap.init( &timer, &mySerial, &xbee, &response, &rx, resources, buf, largeBuf );
  testApp.init( resources, 1, largeBuf );
  mySerial.println("INIT DONE");
#else
  coap.init( &xbee, &response, &rx );
  testApp.init( &coap );
#endif
  digitalWrite( 12, HIGH );
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

