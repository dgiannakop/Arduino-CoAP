#include <coapSensor.h>



class myGETSensor : 
public CoapSensor 
{
public:
  int pin, status;
  myGETSensor(char * name, int pin): 
  CoapSensor(name)
  {
    this->pin = pin;
    this->status = 0;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {

    *output_data_len = sprintf( (char*)output_data, "%d", this->status ); 
  }
  void check(void)
  {
    static unsigned long timestamp = 0;
    if(millis() - timestamp > 500)
    {
      int newStatus = analogRead(this->pin);  // read the value from the sensor
      newStatus = map(newStatus, 0, 1024, 0, 5000)/10;  
      if(newStatus != this->status)
      {
        this->changed = true;
        this->status = newStatus;
      }
      timestamp = millis();
    }
  }
};
