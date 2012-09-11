  #ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <coap.h>

// your app must have an init function where your resource is registered in coap
// callback functions must be char* function(uint8_t method)
// with method you can implement different operations for GET/PUT/POST/DELETE requests

class App
{
   public:
      void init( Coap* coap );
//      coap_status_t lightZones( int zone, uint8_t method, uint8_t* output_data, size_t* output_len );
      coap_status_t lightZones(int zone,uint8_t method, uint8_t* input_data, uint8_t* output_data, size_t* output_data_len);
      coap_status_t lz0( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t lz1( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t lz2( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t lz3( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t lz4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t light( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t pir( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t temperature( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      coap_status_t ch4( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
      

void setLamp(int lamp,uint8_t state)
{
    lampStatuses[lamp]=state;
    digitalWrite(lamp+3, state);
}

 void checkSensors(void){
//  if (!sensorsExist) return;
    checkPir();
    checkLight();
    checkTemp();
    checkMethane();
}


   private:
     uint8_t lampStatuses[5];
     uint8_t ledState;
     Coap* coap_;
     
     uint8_t relayCheckPin;
     uint8_t numOfRelays;
     uint8_t pirPin;
     uint8_t pirStatus;
     uint8_t sensorsCheckPin ;
     bool sensorsExist;
//     uint8_t heaterPin;
//     uint8_t securityPin;

     uint8_t tempPin ;
     uint8_t lightPin ;
     uint8_t methanePin;
     uint8_t carbonPin;
    int tempValue;
    int lightValue;
    int methaneValue;
    int carbonValue;
    
    my_delegate_t  delegate[9];
  //  my_delegate_t delegate0, delegate1, delegate2, delegate3,delegate4;




inline void checkPir(void)
{
  static unsigned long pirTimestamp = 0;
  if(millis() - pirTimestamp > 500)
  {
    
    int newPirStatus = digitalRead(pirPin); // read the value from the sensor
    pirStatus = newPirStatus;
    pirTimestamp = millis();
  }
}

inline void checkLight(void)
{
   // for light sensor

  static unsigned long lightTimestamp = 0;
  if(millis() - lightTimestamp > 3 * 60000)
  {
    lightValue = analogRead(lightPin);  // read the value from the sensor
    lightTimestamp = millis();
  }
  
}

inline void checkMethane(void)
{
   // for temp sensor

  static unsigned long methaneTimestamp = 0;
  if(millis() - methaneTimestamp > 3 * 60000)
  {
    methaneValue = analogRead(methanePin);  // read the value from the sensor
    methaneTimestamp = millis();
  }
}


inline void checkTemp(void)
{
   // for temp sensor

  static unsigned long tempTimestamp = 0;
  if(millis() - tempTimestamp > 3 * 60000)
  {
    uint8_t value = analogRead(tempPin);  // read the value from the sensor
    tempValue = map(value, 0, 1024, 0, 5000)/10;  
    tempTimestamp = millis();
  }
}

     
     
     

};
#endif
