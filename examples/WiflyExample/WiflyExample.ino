/**
 *
 */
//Uncomment for DEBUG in Serial Port
//#define DEBUG
#define WIFLY

#ifdef WIFLY

#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#include <WiFlyHQ.h>

WiFly wifly;

/* Change these to match your WiFi network */
const char mySSID[] = "qopwi-fiwlan";
const char myPassword[] = "password";

#else
//Ethernet Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
byte mac[] = {
  0xAA, 0xAD, 0xBA, 0xED, 0xa3, 0xED};
IPAddress ip(150, 140, 5, 67);
unsigned int localPort = 5683;      // local port to listen on
EthernetUDP EthUDP;
#endif 


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
#ifdef WIFLY
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(13,HIGH);

  Serial.begin(9600);
  wifly.begin(&Serial, NULL);



  /* Join wifi network if not already associated */
  //if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
//    wifly.setIP("192.168.1.6");
//    wifly.setNetmask("255.255.255.0");
//    wifly.setGateway("192.168.1.254");
//    wifly.setDNS("8.8.8.8");
    wifly.enableDHCP();
    wifly.save();

  //}
  wifly.join();
  wifly.setBroadcastInterval(0);	// Turn off UPD broadcast

  //wifly.terminal();

  wifly.setDeviceID("Wifly-CoAP");

  if (wifly.isConnected()) {
    wifly.close();
  }
  delay(1000);
  digitalWrite(13,LOW);

  wifly.setProtocol(WIFLY_PROTOCOL_UDP);
  if (wifly.getPort() != 5683) {
    wifly.setPort(5683);
    /* local port does not take effect until the WiFly has rebooted (2.32) */
    wifly.save();
    wifly.reboot();
  digitalWrite(13,HIGH);
    delay(3000);
  digitalWrite(13,LOW);
  }

  //Init Coap
  //coap.init(&wifly);
#else
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
#endif
/*
  char name[3];  
  name[1]='S';     
  name[2]='\0';
  for (int i=0;i<CONF_MAX_RESOURCES;i++){
    name[0]=0x30+i;
    CoapSensor * example_analog_resource11 = new analogSensor(name,A0);
    coap.add_resource(example_analog_resource11);
  } 



  DBG(
  Serial.println("Setup...Done!");
  )*/
  }

  void loop()
  {
    // nothing else should be done here. CoAP service is running
    // if there is a request for your resource, your callback function will be triggered
    //coap.handler();
     digitalWrite(13,HIGH);
	  delay(100);
	  digitalWrite(13,LOW);
	  delay(100);
	
    int mbyte=-1;
    do{
      mbyte = wifly.read();
      if (mbyte!=-1){
	  digitalWrite(12,HIGH);
	  delay(100);
	  digitalWrite(12,LOW);
	  delay(100);

      }
    }while(mbyte!=-1);
  }












