/**
 *
 */
//Uncomment for DEBUG in Serial Port
#define DEBUG

//Ethernet Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
byte mac[] = {
  0xAA, 0xAD, 0xBA, 0xED, 0xa3, 0xED};
IPAddress ip(150, 140, 5, 67);
unsigned int localPort = 5683;      // local port to listen on
EthernetUDP EthUDP;

//Coap Library
#include <coap.h>
#include "GetSensor.h"
#include "PostSensor.h"
#include "AnalogSensor.h"
Coap coap;
//#define ALL

//Runs only once
void setup()
{
  DBG(
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Setup...");
  )

    //pinMode(9,OUTPUT);



  //Start UDP server
  Ethernet.begin(mac,ip);
  EthUDP.begin(localPort);

  delay(10000);
  //Init Coap
  coap.init(&Ethernet, &EthUDP);
    char name[3];  
     name[1]='S';     name[2]='\0';
  for (int i=0;i<CONF_MAX_RESOURCES;i++){
    name[0]=0x30+i;
    CoapSensor * example_analog_resource11 = new analogSensor(name,A0);
    coap.add_resource(example_analog_resource11);
  } 



  Serial.println("Setup...Done!");

  DBG(
  Serial.println("Setup...Done!");
  )
  }

  void loop()
  {
    // nothing else should be done here. CoAP service is running
    // if there is a request for your resource, your callback function will be triggered
    //digitalWrite(9,HIGH);
    //delay(100);
    //digitalWrite(9,LOW);
    coap.handler();
  }








