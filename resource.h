#ifndef RESOURCE_H
#define RESOURCE_H

#include <Arduino.h>
#include "FastDelegate.h"
#include "vector.h"
#include "packet.h"

typedef fastdelegate::FastDelegate6<uint8_t, uint8_t*, size_t, uint8_t*, size_t*, queries_t, coap_status_t> my_delegate_t;

class CoapResource
{
   public:
      CoapResource( String name, uint8_t methods, my_delegate_t delegate, bool fast_resource, uint16_t notify_time, uint8_t content_type );
      coap_status_t execute( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries );
      void set_notify_time( uint16_t notify_time );
      void set_interrupt_flag( bool flag );
      bool is_set();
      String name();
      uint8_t name_length();
      void nameToStr( char* buf, size_t len );
      uint8_t method_allowed( uint8_t method );
      uint16_t notify_time_w();
      uint8_t resource_len();
      bool fast_resource();
      uint8_t content_type();
      bool interrupt_flag_w();
   private:
      bool is_set_;
      my_delegate_t del_;
      String name_;
      uint8_t methods_;
      uint16_t notify_time_;
      bool fast_resource_;
      uint8_t resource_len_;
      uint8_t content_type_;
      bool interrupt_flag_;
};
#endif

