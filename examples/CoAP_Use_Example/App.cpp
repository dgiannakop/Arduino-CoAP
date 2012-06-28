#include "App.h"

void App::init( Coap* coap )
{
   coap_ = coap;
   my_delegate_t delegate;

   ledState = 0;

   // first we create a delegate for our callback function
   delegate = fastdelegate::MakeDelegate( this, &App::test );
   coap->add_resource("test", GET | PUT, delegate, true, 30, TEXT_PLAIN);
}
 
coap_status_t App::test( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries)
{
    *output_data_len = sprintf( (char*)output_data, "working" );
    /*
    char char_buffer[query_value.length()];
    query_value.toCharArray(char_buffer, query_value.length());
    int i = int(char_buffer);
    coap_->update_resource("test", GET | PUT, true, i, TEXT_PLAIN);
   //resources_[rid].set_payload_len( sprintf( data_, "working" ) );
   */
   return CONTENT;
}
 
