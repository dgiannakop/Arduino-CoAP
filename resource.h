#ifndef RESOURCE_H
#define RESOURCE_H

#include <Arduino.h>
#include "packet.h"
#include "coapSensor.h"

class CoapResource
{
   public:
		
      CoapResource(){};
      /**
       * Resource Constructor. User to pass as parameters all required fields: 
       *
       */
      CoapResource( String name, uint8_t methods, CoapSensor * sensor, bool fast_resource, uint16_t notify_time, uint8_t content_type );
      /**
       * Called when a resouce is to be evaluated again.
       */
      coap_status_t execute( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries );
      /**
       * Sets a new notification interval for the current resource.
       */
      void set_notify_time( uint16_t notify_time );
	  /**
	   * Sets the Interupt flag.
	   * Notes a change in the Resource's value.
	   */
      void set_interrupt_flag( bool flag );
	  /**
	   * 
	   */
      bool is_set();
      /**
       * String representation of the resource's name.
       */
      String name();
      /**
       * Length of the resource's name.
       */
      uint8_t name_length();
      void nameToStr( char* buf, size_t len );
      /**
       * Check if the resource allows the method.
       */
      uint8_t method_allowed( uint8_t method );
      
      uint16_t notify_time_w();
      uint8_t resource_len();
      bool fast_resource();
      uint8_t content_type();
      bool interrupt_flag_w();
   private:
      bool is_set_;
       CoapSensor *del_;
      String name_;
      uint8_t methods_;
      uint16_t notify_time_;
      bool fast_resource_;
      uint8_t resource_len_;
      uint8_t content_type_;
      bool interrupt_flag_;
};
#endif

