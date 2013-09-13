#include <CoapSensor.h>

class getSensor : public CoapSensor
{
public:
  int status;
  getSensor(String name): CoapSensor(name)
  {
    this->status = 42;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "%d", this->status );
  }
};
