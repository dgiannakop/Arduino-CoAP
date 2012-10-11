#include <coapSensor.h>

class myPOSTSensor : 
public CoapSensor {

public:  
  myPOSTSensor(String name , uint8_t method , boolean fast , uint16_t notify_time, uint8_t content_type , int pin){
    pin_ = pin;
    name_ = name;
    method_ = method;
    fast_ = fast;
    notify_time_ = notify_time;
    content_type_ = content_type;
    value=0;
  }

  //Handler function
  coap_status_t callback(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
    //Response to the GET request
    if( method == GET )  {
      *output_data_len = sprintf( (char*)output_data, "%s", value?"on":"off" ); 
      return CONTENT;
    }
    else if (method==POST){
      
      if ((char)input_data[0] == 'y'){
        digitalWrite(pin_,HIGH);
        value=1;
      }
      else if ((char)input_data[0] == 'n'){
        digitalWrite(pin_,LOW);
        value=0;
      }
      *output_data_len = sprintf( (char*)output_data, "%s", value?"on":"off"  ); 
      return CHANGED;      
    }
  }


  uint8_t method(){
    return method_;
  }

  String name(){
    return name_;
  }

  bool fast(){
    return fast_;
  }

  uint16_t notify_time(){
    return notify_time_;
  }

  uint8_t content_type(){
    return content_type_;
  }

private :
  int pin_;
  int value;
  String name_;
  uint8_t method_;
  boolean fast_;
  uint16_t notify_time_;
  uint8_t content_type_;

};
