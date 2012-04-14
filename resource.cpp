#include "resource.h"

void CoapResource::init()
{
   uint8_t i;
   is_set_ = false;
   for( i = 0; i < CONF_MAX_RESOURCE_QUERIES; i++ )
   {
      methods_[i] = 0x00;
      q_name_[i] = NULL;
   }
}

void CoapResource::reg_callback( my_delegate_t delegate, uint8_t qid )
{
   del_[qid] = delegate;
}

void CoapResource::execute( uint8_t qid, uint8_t par )
{
   payload_ = NULL;
   if( del_[qid] )
   {
      payload_ = del_[qid]( par );
      input_data_ = NULL;
   }
}

void CoapResource::reg_resource( String name, bool fast_resource, uint16_t notify_time, uint8_t resource_len, uint8_t content_type )
{
   name_ = name;
   is_set_ = true;
   fast_resource_ = fast_resource;
   resource_len_ = resource_len;
   content_type_ = content_type;
   notify_time_ = notify_time;
   interrupt_flag_ = false;
}

void CoapResource::reg_query( uint8_t qid, String name )
{
   q_name_[qid] = name;
}

uint8_t CoapResource::has_query( String query )
{
   uint8_t qid;
   for( qid = 1; qid < CONF_MAX_RESOURCE_QUERIES; qid++ )
   {
      if ( query.length() > 0 && query == q_name_[qid] )
      {
         return qid;
      }
   }
   return 0;
}
void CoapResource::set_method( uint8_t qid, uint8_t method )
{
   methods_[qid] |= 1L << method;
}
void CoapResource::set_notify_time( uint16_t notify_time )
{
   notify_time_ = notify_time;
}
void CoapResource::set_interrupt_flag( bool flag )
{
   interrupt_flag_ = flag;
}
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

uint8_t CoapResource::method_allowed( uint8_t qid, uint8_t method )
{
   return methods_[qid] & 1L << method;
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
