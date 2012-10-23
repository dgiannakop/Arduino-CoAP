#include <CoapSensor.h>

class mySensor : public CoapSensor 
{
public:
  mySensor(String name): CoapSensor(name)
  {
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
  }
  void set_value(uint8_t* input_data, size_t input_data_len)
  {
  }
};
