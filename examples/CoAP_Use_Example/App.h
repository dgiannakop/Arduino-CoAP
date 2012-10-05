  #ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <coap.h>

// your app must have an init function where your resource is registered in coap
// callback functions must be char* function(uint8_t method)
// with method you can implement different operations for GET/PUT/POST/DELETE requests

class App
{
   public:
      void init( Coap* coap );
      coap_status_t lightZones( int zone, uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t l0( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t l1( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t l2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t l3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t l4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t light( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t pir( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t temperature( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t ch4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      

void setLamp(int lamp, int value)
{
  lampStatuses[lamp] = value;
  digitalWrite(lamp+3, lampStatuses[lamp]);
}


   private:
     uint8_t lampStatuses[5];
     uint8_t ledState;
     Coap* coap_;
     
     uint8_t relayCheckPin;
     uint8_t numOfRelays;
     uint8_t pirPin;
     uint8_t pirStatus;
     uint8_t sensorsCheckPin ;
     bool sensorsExist;
     uint8_t heaterPin;
     uint8_t securityPin;

     uint8_t tempPin ;
     uint8_t lightPin ;
     uint8_t methanePin;
     uint8_t carbonPin;
    int tempValue;
    int lightValue;
    int methaneValue;
    int carbonValue;



     
     
     

};
#endif
