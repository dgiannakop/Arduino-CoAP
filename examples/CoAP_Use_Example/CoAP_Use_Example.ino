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
#include <BaseRouting.h>
#include <TreeRouting.h>
#include <NonRouting.h>
//Message Routing
BaseRouting * routing;

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



/**
 */
void radio_callback(uint16_t sender, byte* payload, unsigned int length) {
  coap.receiver(payload, sender, length);
}

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

#ifdef USE_TREE_ROUTING
  routing = new TreeRouting(&xbee);
#else 
  routing = new NonRouting(&xbee);
#endif 
  routing->set_sink(false);

  uint16_t address = xbee.getMyAddress(); //fix 4hex digit address
  uint8_t * bit = ((uint8_t*) & address);
  uint8_t mbyte = bit[1];
  uint8_t lbyte = bit[0];
  bit[0] = mbyte;
  bit[1] = lbyte;
  routing->set_my_address(address);
  routing->set_message_received_callback(radio_callback);
  //routing->setXbeeRadio(&xbee);
  // init coap service 
  coap.init(address, routing);

  //add the resourse resGET
  coap.add_resource(&aSensor);
  coap.add_resource(&bSensor);
  //add the resources resGET-POST
  //coap.add_resource("resGET-POST"  , GET | POST, &handlerGET_POST, true, 20, TEXT_PLAIN);


}

void loop()
{
  routing->loop();
  //run the handler on each loop to respond to incoming requests
  coap.handler();
}
