#include <CoapSensor.h>

class zoneSensor : public CoapSensor 
{
public:
int pin, status;
  zoneSensor(String name, int pin): CoapSensor(name)
  {
    this->pin = pin;
    this->status = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    this->status = digitalRead(this->pin);
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
    digitalWrite(pin, status);
  }
};

class switchSensor : public CoapSensor 
{
public:
int pin, status;
  switchSensor(String name, int pin): CoapSensor(name)
  {
    this->pin = pin;
    pinMode(pin, INPUT);
    this->status = 0;
  }
  switchSensor(String name, int pin, int pullup): CoapSensor(name)
  {
    this->pin = pin;
    pinMode(pin, INPUT);
    digitalWrite(pin, pullup);
    this->status = 0;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "%d", this->status ); 
  }
  
  void check()
  {
    int newStatus = digitalRead(this->pin);
    if(newStatus != this->status)
    {
      this->changed = true;
      this->status = newStatus;
    }
  }
};

class lightSensor : public CoapSensor 
{
public:
int pin, status;
  lightSensor(String name, int pin): CoapSensor(name)
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
      if(newStatus > this->status + 10 || newStatus < this->status - 10 )
      {
        this->changed = true;
        this->status = newStatus;
      }
      timestamp = millis();
    }
  }
};

class temperatureSensor : public CoapSensor 
{
public:
int pin, status;
  temperatureSensor(String name, int pin): CoapSensor(name)
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

class pirSensor : public CoapSensor 
{
public:
int pin, status;
  pirSensor(String name, int pin): CoapSensor(name)
  {
    this->pin = pin;
    this->status = LOW;
    digitalWrite(pin, HIGH);
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
      int newStatus = !digitalRead(this->pin); // read the value from the sensor
      if(newStatus != this->status)
      {
        this->changed = true;
        this->status = newStatus;
      }
      timestamp = millis();
    }
  }
};

class methaneSensor : public CoapSensor 
{
public:
int pin, status;
  methaneSensor(String name, int pin): CoapSensor(name)
  {
    this->pin = pin;
    this->status = LOW;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "%d", this->status ); 
  }
  
  void check(void)
  {
    static unsigned long timestamp = 0;
    if(millis() - timestamp > 150000)
    {
      this->status = analogRead(pin);  // read the value from the sensor
      this->changed = true;
      timestamp = millis();
    }
  }
};

class carbonSensor : public CoapSensor 
{
public:
int pin, heater_pin, status, heater;
  carbonSensor(String name, int pin, int heater_pin): CoapSensor(name)
  {
    this->pin = pin;
    this->heater_pin = heater_pin;
    pinMode(heater_pin, OUTPUT);
    digitalWrite(heater_pin, LOW);
    this->status = LOW;
    this->heater = LOW;
  }
  void get_value( uint8_t* output_data, size_t* output_data_len)
  {
    *output_data_len = sprintf( (char*)output_data, "%d", this->status ); 
  }
  
  void check(void)
  {
    static unsigned long timestamp = 0;
    if(millis() - timestamp > 30000)
    {
      static unsigned int count = 0;
      if(count == 0)
      {
        analogWrite(heater_pin, 56);
      }
      else if(count == 3)
      {
        digitalWrite(heater_pin, HIGH);
      }
      else if(count == 5)
      {
        this->status = analogRead(pin);  // read the value from the sensor
        this->changed = true;
        count = -1;
      }
      count++;
      timestamp = millis();
    }
  }
};

