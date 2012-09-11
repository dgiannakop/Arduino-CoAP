#include "App.h"

void App::init( Coap* coap )
{
  relayCheckPin = A4;
  numOfRelays = 0;
  pirPin = 9;

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
  
  
  pinMode(sensorsCheckPin, INPUT);
  digitalWrite(sensorsCheckPin, HIGH);
  sensorsExist = !digitalRead(sensorsCheckPin);


  if((sensorsExist)||(true))
  {
    pinMode(pirPin, INPUT);
    pinMode(tempPin, INPUT);
    pinMode(lightPin, INPUT);
    pinMode(methanePin, INPUT);

    digitalWrite(pirPin, HIGH);
//    digitalWrite(securityPin, HIGH);


  
  delegate[5] = fastdelegate::MakeDelegate( this, &App::pir );
  delegate[6] = fastdelegate::MakeDelegate( this, &App::light );
  delegate[7] = fastdelegate::MakeDelegate( this, &App::temperature );
  delegate[8] = fastdelegate::MakeDelegate( this, &App::ch4 );    
    
  coap->add_resource("pir"    , GET      , delegate[5], true, 20, TEXT_PLAIN);
  coap->add_resource("light"  , GET      , delegate[6], true, 20, TEXT_PLAIN);
  coap->add_resource("temp"    , GET, delegate[7], true, 20, TEXT_PLAIN);
  coap->add_resource("ch4", GET, delegate[8], true, 20, TEXT_PLAIN);
  }  

  coap_ = coap;
  



  lampStatuses[0] = 0;
  lampStatuses[1] = 0;
  lampStatuses[2] = 0;
  lampStatuses[3] = 0;
  lampStatuses[4] = 0;

  ledState = 0;

  // first we create a delegate for our callback function
  delegate[0] = fastdelegate::MakeDelegate( this, &App::lz0 );
  delegate[1] = fastdelegate::MakeDelegate( this, &App::lz1 );
  delegate[2] = fastdelegate::MakeDelegate( this, &App::lz2 );
  delegate[3] = fastdelegate::MakeDelegate( this, &App::lz3 );
  delegate[4] = fastdelegate::MakeDelegate( this, &App::lz4 );


  
  coap->add_resource("lz0"    , GET | POST, delegate[0], true, 20, TEXT_PLAIN);
  coap->add_resource("lz1"    , GET | POST, delegate[1], true, 20, TEXT_PLAIN);
  coap->add_resource("lz2"    , GET | POST, delegate[2], true, 20, TEXT_PLAIN);
  coap->add_resource("lz3"    , GET | POST, delegate[3], true, 20, TEXT_PLAIN);
  coap->add_resource("lz4"    , GET | POST, delegate[4], true, 20, TEXT_PLAIN);


}


//coap_status_t App::lightZones(int zone, uint8_t method, uint8_t* output_data, size_t* output_data_len){
inline coap_status_t App::lightZones(int zone,uint8_t method, uint8_t* input_data, uint8_t* output_data, size_t* output_data_len){
  if( method == GET )  {
    output_data[0] = 0x30 + lampStatuses[zone];
    *output_data_len = 1;
    return CONTENT;
  }
  else if ( method == POST ) {
    setLamp( zone , *input_data-0x30);
    output_data[0] = 0x30 + lampStatuses[zone];
    *output_data_len = 1;
    //*output_data_len = sprintf( (char*)output_data, "%d", lampStatuses[zone] );
    return CHANGED;
  }
}

coap_status_t App::lz0( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 0, method, input_data,output_data,  output_data_len);
}
coap_status_t App::lz1( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method, input_data,output_data,  output_data_len);
}
coap_status_t App::lz2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 2, method, input_data,output_data,  output_data_len);
}
coap_status_t App::lz3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 3, method, input_data,output_data,  output_data_len);
}
coap_status_t App::lz4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 4, method, input_data,output_data,  output_data_len);
}
/*
coap_status_t App::lz0( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 0, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::lz1( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 1, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::lz2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 2, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::lz3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 3, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
coap_status_t App::lz4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
return lightZones( 4, method,  input_data,  input_data_len,  output_data,  output_data_len,  queries);
}
*/
coap_status_t App::pir(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
//    *output_data_len = sprintf( (char*)output_data, "%d",pirStatus ); 
    output_data[0] = 0x30 + pirStatus;
    return CONTENT;
  }
}

coap_status_t App::light(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    *output_data_len = sprintf( (char*)output_data, "%d" ,lightValue); 
    return CONTENT;
  }
}

coap_status_t App::temperature(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    *output_data_len = sprintf( (char*)output_data, "%d" ,tempValue); 
    return CONTENT;
  }
}

coap_status_t App::ch4(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries){
  if( method == GET )  {
    *output_data_len = sprintf( (char*)output_data, "%d" ,methanePin); 
    return CONTENT;
  }
}
