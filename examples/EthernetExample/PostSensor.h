#include <CoapSensor.h>

class postSensor : public CoapSensor
{

public:
int status;
  postSensor(String name): CoapSensor(name)
  {
    this->status = 0;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "%d", this->status );
  }
  void set_value(uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len)
  {
    this->set(*input_data-0x30);
    output_data[0] = 0x30 + status;
    *output_data_len = 1;
  }
  void set(uint8_t value)
  {
    this->status = value;
  }
};
