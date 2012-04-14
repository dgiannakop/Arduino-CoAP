#ifndef RESOURCE_H
#define RESOURCE_H

#include <Arduino.h>
#include "coap_conf.h"
#include "FastDelegate.h"

typedef fastdelegate::FastDelegate1<uint8_t, char *> my_delegate_t;

class CoapResource
{
   public:
      void init();
      void reg_callback(my_delegate_t delegate, uint8_t qid );
      void execute( uint8_t qid, uint8_t par );
      void reg_resource( String name, bool fast_resource, uint16_t notify_time, uint8_t resource_len, uint8_t content_type );
      void reg_query( uint8_t qid, String name );
      uint8_t has_query( String query );
      void set_method( uint8_t qid, uint8_t method );
      void set_notify_time( uint16_t notify_time );
      void set_interrupt_flag( bool flag );
      void set_input_data( uint8_t * put_data );
      void set_input_data_len( uint8_t put_data_len );
      void set_payload_len( uint8_t payload_len );
      bool is_set();
      String name();
      uint8_t name_length();
      void nameToStr( char* buf, size_t len );
      uint8_t method_allowed( uint8_t qid, uint8_t method );
      uint16_t notify_time_w();
      uint8_t resource_len();
      bool fast_resource();
      uint8_t content_type();
      bool interrupt_flag_w();
      char* payload();
      uint8_t payload_len_w();
      uint8_t * input_data_w();
      uint8_t input_data_len_w();
   private:
      bool is_set_;
      my_delegate_t del_[CONF_MAX_RESOURCE_QUERIES];
      String name_;
      String q_name_[CONF_MAX_RESOURCE_QUERIES];
      uint8_t methods_[CONF_MAX_RESOURCE_QUERIES];
      uint16_t notify_time_;
      bool fast_resource_;
      uint8_t resource_len_;
      uint8_t content_type_;
      bool interrupt_flag_;
      char *payload_;
      uint8_t payload_len_;
      uint8_t *input_data_;
      uint8_t input_data_len_;
};
#endif
