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
      coap_status_t test( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t test2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t test3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      void setLamp(int, int);
   private:
     uint8_t lampPins[5];
     uint8_t lampStatuses[5];
       uint8_t ledState;
      Coap* coap_;
};
#endif
