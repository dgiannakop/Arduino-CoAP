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
      void init( resource_t* resources, uint8_t rid, char* data );
      char* get_temp( uint8_t method );
      char* temp_status( uint8_t method );
      char* change_observe_timer(uint8_t method );
   private:
      resource_t* resources_;
      uint8_t rid_;
      char* data_;
};
#endif
