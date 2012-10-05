#include "App.h"

void App::init( Coap* coap )
{
  relayCheckPin = A4;
  numOfRelays = 0;
  pirPin = 9;
  heaterPin = 10;
  securityPin = 11;

  sensorsCheckPin = 12;
  tempPin = A0;
  lightPin = A1;
  methanePin = A2;
  carbonPin = A3;
  sensorsExist = false;

  tempValue=0;
  lightValue=0;
  methaneValue=0;
  carbonValue=0;

  coap_ = coap;
  my_delegate_t delegate0, delegate1, delegate2, delegate3,delegate4,delegate5;
  my_delegate_t  delegate6, delegate7;


  lampStatuses[0] = 0;
  lampStatuses[1] = 0;
  lampStatuses[2] = 0;
  lampStatuses[3] = 0;

  ledState = 0;

  // first we create a delegate for our callback function
  delegate0 = fastdelegate::MakeDelegate( this, &App::l0 );
  delegate1 = fastdelegate::MakeDelegate( this, &App::l1 );
  delegate2 = fastdelegate::MakeDelegate( this, &App::l2 );
  delegate3 = fastdelegate::MakeDelegate( this, &App::l3 );
  delegate4 = fastdelegate::MakeDelegate( this, &App::l4 );

  delegate5 = fastdelegate::MakeDelegate( this, &App::pir );
  delegate6 = fastdelegate::MakeDelegate( this, &App::light );
  delegate7 = fastdelegate::MakeDelegate( this, &App::temperature );
//  delegate8 = fastdelegate::MakeDelegate( this, &App::ch4 );
  
  coap->add_resource("lz0"    , GET | PUT, delegate0, true, 20, TEXT_PLAIN);
  coap->add_resource("lz1"    , GET | PUT, delegate1, true, 20, TEXT_PLAIN);
  coap->add_resource("lz2"    , GET | PUT, delegate2, true, 20, TEXT_PLAIN);
  coap->add_resource("lz3"    , GET | PUT, delegate3, true, 20, TEXT_PLAIN);
  coap->add_resource("lz4"    , GET | PUT, delegate4, true, 20, TEXT_PLAIN);

  coap->add_resource("pir"    , GET      , delegate5, true, 20, TEXT_PLAIN);
  coap->add_resource("light"  , GET      , delegate6, true, 20, TEXT_PLAIN);
  coap->add_resource("temperature"    , GET, delegate7, true, 20, TEXT_PLAIN);
//  coap->add_resource("ch4", GET, delegate8, true, 20, TEXT_PLAIN);

}


coap_status_t App::lightZones(int zone, uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    *output_data_len = sprintf( (char*)output_data, "%d", lampStatuses[zone] ); 
    return CONTENT;
  }
  else if ( method == PUT ) {
    setLamp( zone, *input_data - 0x30 );
    *output_data_len = sprintf( (char*)output_data, "%d", lampStatuses[zone] );
    return CHANGED;
  }
}

coap_status_t App::l0( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 0, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::l1( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::l2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::l3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::l4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}

coap_status_t App::pir(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    int pirstate=0;
    *output_data_len = sprintf( (char*)output_data, "%d",pirstate ); 
    return CONTENT;
  }
}

coap_status_t App::light(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    int lux=0;
    *output_data_len = sprintf( (char*)output_data, "%d" ,lux); 
    return CONTENT;
  }
}

coap_status_t App::temperature(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    int temp=0;
    *output_data_len = sprintf( (char*)output_data, "%d" ,temp); 
    return CONTENT;
  }
}

coap_status_t App::ch4(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    int temp=0;
    *output_data_len = sprintf( (char*)output_data, "%d" ,temp); 
    return CONTENT;
  }
}
