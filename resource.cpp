#include "resource.h"

CoapResource::CoapResource( String name, uint8_t methods, my_delegate_t delegate, bool fast_resource, uint16_t notify_time, uint8_t content_type )
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

void CoapResource::execute( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries )
{
   payload_ = NULL;
   if( del_ )
   {
      output_data = (uint8_t*)del_( method, input_data, input_data_len, output_data_len, queries);
   }
   //return payload_;
}

void CoapResource::set_notify_time( uint16_t notify_time )
{
   notify_time_ = notify_time;
}
void CoapResource::set_interrupt_flag( bool flag )
{
   interrupt_flag_ = flag;
}
/*
void CoapResource::set_input_data( uint8_t * put_data )
{
   input_data_ = put_data;
}

void CoapResource::set_input_data_len( uint8_t put_data_len )
{
   input_data_len_ = put_data_len;
}

void CoapResource::set_payload_len( uint8_t payload_len )
{
   payload_len_ = payload_len;
}
*/
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
/*
char* CoapResource::payload()
{
   return payload_;
}
uint8_t CoapResource::payload_len_w()
{
   return payload_len_;
}
uint8_t* CoapResource::input_data_w()
{
   return input_data_;
}
uint8_t CoapResource::input_data_len_w()
{
   return input_data_len_;
}
*/
