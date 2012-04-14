#include "App.h"

void App::init( resource_t* resources, uint8_t rid, char* data )
{
   resources_ = resources;
   data_ = data;
   my_delegate_t delegate;
   rid_ = rid;

   // first we create a delegate for our callback function
   delegate = fastdelegate::MakeDelegate( this, &App::get_temp );
   // set the method, arguments are: QuerryID, Method
   resources[rid_].set_method( 0, GET );
   // pass the delegate to the object, for the desired QuerryID
   resources[rid_].reg_callback( delegate, 0 );
   // register the resource with arguments: Name, FastRespone, Default NotifyTime, Expected Length, ContentType
   // Zero default notification time, disables observing capability
   resources[rid_].reg_resource( "sensors/temp", true, 120, 5, TEXT_PLAIN );
   //resources[rid_+1].reg_resource( "sensors/light", true, 90, 5, TEXT_PLAIN );
   //resources[rid_+2].reg_resource( "test/large_uri/for_testing", true, 90, 5, TEXT_PLAIN );

   delegate = fastdelegate::MakeDelegate( this, &App::temp_status );
   resources[rid].set_method( 1, GET );
   resources[rid].set_method( 1, PUT );
   resources[rid].reg_callback( delegate, 1 );
   // QuerryID, QuerryName
   resources[rid].reg_query( 1, "act=status" );

   delegate = fastdelegate::MakeDelegate( this, &App::change_observe_timer );
   resources[rid].set_method( 2, GET );
   resources[rid].set_method( 2, PUT );
   resources[rid].reg_callback( delegate, 2 );
   resources[rid].reg_query( 2, "act=observe" );
}

char* App::get_temp( uint8_t method )
{
   resources_[rid_].set_payload_len(sprintf(data_, "working" ));
   return data_;
}

char* App::temp_status( uint8_t method )
{
   resources_[rid_].set_payload_len(sprintf(data_, "true" ));
   return data_;
}

char* App::change_observe_timer( uint8_t method )
{
   uint8_t* data = resources_[rid_].input_data_w();
   uint8_t len = resources_[rid_].input_data_len_w();
   uint16_t value = 0;
   uint8_t i;
   if ( method == GET )
   {
      resources_[rid_].set_payload_len(sprintf( data_, "%d", resources_[rid_].notify_time_w() ));
   }
   if ( method == PUT )
   {
      for( i = 0; i < len; i++ )
      {
         value = value * 10 + ( data[i] - 0x30 );
      }
      resources_[rid_].set_notify_time( value );
      resources_[rid_].set_payload_len(sprintf( data_, "set:%d", value ));
   }
   return data_;
}

// in case of a new value, you want to notify your observers.
// from your routine you must call "coap_notify_from_interrupt( rid_ );"
// observers will be notified


