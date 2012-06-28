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
      void init( Coap* coap, char* data );
      char* test( uint8_t method, uint8_t* input_data, size_t input_data_len, size_t* outpu_len, queries_t queries );
   private:
       uint8_t ledState;
      Coap* coap_;
      char* data_;
};
#endif
