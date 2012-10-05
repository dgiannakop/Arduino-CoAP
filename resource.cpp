#include "resource.h"
CoapResource::CoapResource( String name, uint8_t methods, coap_status_t (*delegate) (uint8_t, uint8_t*, size_t, uint8_t*, size_t*, queries_t), bool fast_resource, uint16_t notify_time, uint8_t content_type )
{
   name_ = name;
   methods_ = methods;
   del_ = delegate;
   is_set_ = true;
   fast_resource_ = fast_resource;
   content_type_ = content_type;
   notify_time_ = notify_time;
   interrupt_flag_ = false;
}

coap_status_t CoapResource::execute( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries )
{
   if( del_ )
   {
      if (method == 3)
         method = 4;
      else if (method == 4)
         method = 8;
      return del_( method, input_data, input_data_len, output_data, output_data_len, queries );
   }
}

void CoapResource::set_notify_time( uint16_t notify_time )
{
   notify_time_ = notify_time;
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
   return name_;
}

uint8_t CoapResource::name_length()
{
   return name_.length();
}

void CoapResource::nameToStr( char* buf, size_t len )
{
   name_.toCharArray( buf, len );
}

uint8_t CoapResource::method_allowed( uint8_t method )
{
   if (method == 3)
      method = 4;
   else if (method == 4)
      method = 8;
   return methods_ & method;
   //return methods_[qid] & 1L << method-1;
}

uint16_t CoapResource::notify_time_w()
{
   return notify_time_;
}

uint8_t CoapResource::resource_len()
{
   return resource_len_;
}

bool CoapResource::fast_resource()
{
   return fast_resource_;
}

uint8_t CoapResource::content_type()
{
   return content_type_;
}

bool CoapResource::interrupt_flag_w()
{
   return interrupt_flag_;
}
