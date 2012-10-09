#include <coapSensor.h>

class mySensor : public CoapSensor {

  public:
  mySensor(int input){
    value=input;
  }
  
  //Handler function
  coap_status_t callback(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  //Response to the GET request
  if( method == GET )  {
    *output_data_len = sprintf( (char*)output_data, "%d",value ); 
    return CONTENT;
  }
}
  
  private :
  int value;
};
