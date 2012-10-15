#include "resource.h"

CoapResource::CoapResource( String name, uint8_t methods, CoapSensor * sensor, bool fast_resource, uint16_t notify_time, uint8_t content_type )
{
   //name_ = name;
   //methods_ = methods;
   del_ = sensor;
   is_set_ = true;
   //fast_resource_ = fast_resource;
   //content_type_ = content_type;
   //notify_time_ = notify_time;
   interrupt_flag_ = false;
}

coap_status_t CoapResource::execute( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries )
{
   if( del_ )
   {
	   
      if (method == COAP_GET)
         method = GET;
      else if (method == COAP_POST)
         method = POST;
      return del_->callback( method, input_data, input_data_len, output_data, output_data_len, queries );
   }
}

void CoapResource::set_notify_time( uint16_t notify_time )
{
//    notify_time_ = notify_time;
    del_->set_notify_time(notify_time);
}

void CoapResource::set_interrupt_flag( bool flag )
{
   interrupt_flag_ = flag;
}

bool CoapResource::is_set()
{
   return is_set_;
}

String CoapResource::name()
{
   //return name_;
    return del_->get_name();
}

uint8_t CoapResource::name_length()
{
   return del_->get_name().length();
}

void CoapResource::nameToStr( char* buf, size_t len )
{
   return del_->get_name().toCharArray( buf, len );
}

uint8_t CoapResource::method_allowed( uint8_t method )
{
   if (method == 3)
      method = 4;
   else if (method == 4)
      method = 8;
   return del_->get_method() & method;
   //return methods_[qid] & 1L << method-1;
}

uint16_t CoapResource::notify_time_w()
{
  return del_->get_notify_time();
}

// uint8_t CoapResource::resource_len()
// {
//    return resource_len_;
// }

bool CoapResource::fast_resource()
{
   return del_->get_fast();
}

uint8_t CoapResource::content_type()
{
   return del_->get_content_type();
}

bool CoapResource::interrupt_flag_w()
{
   return interrupt_flag_;
}
