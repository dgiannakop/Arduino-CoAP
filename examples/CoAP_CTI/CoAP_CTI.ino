/**
 * Arduino Coap Example Application.
 *
 * This Example creates a CoAP server with 2 resources.
 * resGET : A resource that contains an integer and the GET method is only available.
 *          A GET request returns the value of the value_get variable.
 * resGET-POST : A resource that contains an integer and the GET-POST methods are available.
 *               A GET request returns the value of the value_post variable.
 *               A POST request sets the value of value_post to the sent integer.
 * Both resources are of TEXT_PLAIN content type.
 */

//Include XBEE Libraries
#include <XBee.h>
#include <XbeeRadio.h>

//Include CoAP Libraries
#include <coap.h>
#include "mySensor.h"

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
//Create a reusable response object for responses we expect to handle
XBeeRadioResponse response = XBeeRadioResponse();
//Create a reusable rx16 response object to get the address
Rx16Response rx = Rx16Response();

//CoAP object
Coap coap;

//CoapSensor aSensor = CoapSensor();

//Runs only once

void setup() {
    // comment out for debuging
    xbee.initialize_xbee_module();
    //start our XbeeRadio object and set our baudrate to 38400.
    xbee.begin(38400);
    //Initialize our XBee module with the correct values (using the default channel, channel 12)h
    xbee.init(12);

    // init coap service 
    coap.init(&xbee, &response, &rx);

    add_relays();
    add_sensors();
}

void loop() {
    //run the handler on each loop to respond to incoming requests
    coap.handler();
}

uint8_t getNumOfRelays(int relayCheckPin) {
    uint8_t relays[] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 10; i++) {
	relays[getNumOfRels(relayCheckPin)]++;
    }
    int num = 0;

    for (int i = 1; i < 6; i++) {
	if (relays[i] > relays[i - 1])
	    num = i;
    }
    return num;
}

uint8_t getNumOfRels(int relayCheckPin) {
    int value = analogRead(relayCheckPin);
    delay(10);
    int relNum = 0;
    int distance[5];
    int thresholds[] = {
	0, 342, 512, 614, 683, 732};
    for (int i = 0; i < 6; i++) {
	thresholds[i] < value ? distance[i] = value - thresholds[i] : distance[i] = thresholds[i] - value;
    }

    for (int i = 1; i < 6; i++)
	if (distance[i] < distance[i - 1]) relNum = i;

    return relNum;
}

void add_relays() {
#define RELAY_CHECK_PIN A4
    int numOfRelays = getNumOfRelays(RELAY_CHECK_PIN);
    for (int i = 0; i < numOfRelays; i++) {
#define RELAY_START_PIN 2
	zoneSensor* lzSensor = new zoneSensor(String("lz")+(i + 1), RELAY_START_PIN + i);
	coap.add_resource(lzSensor);
    }
}

void add_sensors() {
#define SENSORS_CHECK_PIN 12
#define SECURITY_PIN 11
#define TEMP_PIN A0
#define LIGHT_PIN A1
#define METHANE_PIN A2
#define CARBON_PIN A3
#define HEATER_PIN 10
#define PIR_PIN 9
    pinMode(SENSORS_CHECK_PIN, INPUT);
    digitalWrite(SENSORS_CHECK_PIN, HIGH);
    bool sensorsExist = !digitalRead(SENSORS_CHECK_PIN);
    if (true) {
	//switchSensor* swSensor = new switchSensor("security", SECURITY_PIN, HIGH);
	//coap.add_resource(swSensor);
	temperatureSensor* tempSensor = new temperatureSensor("temperature", TEMP_PIN);
	coap.add_resource(tempSensor);
	lightSensor* liSensor = new lightSensor("light", LIGHT_PIN);
	coap.add_resource(liSensor);
	//methaneSensor* mh4Sensor = new methaneSensor("methane", METHANE_PIN);
	//coap.add_resource(mh4Sensor);
	//carbonSensor* coSensor = new carbonSensor("carbon", CARBON_PIN, HEATER_PIN);
	//coap.add_resource(coSensor);
	pirSensor* pSensor = new pirSensor("pir", PIR_PIN);
	coap.add_resource(pSensor);
    }
}
