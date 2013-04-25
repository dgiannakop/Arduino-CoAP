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
#include "myGETSensor.h"
#include "myPOSTSensor.h"

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
//Create a reusable response object for responses we expect to handle
XBeeRadioResponse response = XBeeRadioResponse();
//Create a reusable rx16 response object to get the address
Rx16Response rx = Rx16Response();

//CoAP object
Coap coap;

myGETSensor aSensor = myGETSensor("resGET1" , A0);
myPOSTSensor bSensor = myPOSTSensor("resGET-POST" , 3);


//Runs only once
void setup()
{

  pinMode(3, OUTPUT);     
  
  // comment out for debuging
  xbee.initialize_xbee_module();
  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin( 38400 );
  //Initialize our XBee module with the correct values (using the default channel, channel 12)h
  xbee.init(12);

  // init coap service 
  coap.init( &xbee, &response, &rx );

  //add the resourse resGET
  coap.add_resource(&aSensor);
  coap.add_resource(&bSensor);
  //add the resources resGET-POST
  //coap.add_resource("resGET-POST"  , GET | POST, &handlerGET_POST, true, 20, TEXT_PLAIN);


}

void loop()
{
  //run the handler on each loop to respond to incoming requests
  coap.handler();
}
