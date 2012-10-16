#ifndef COAP_SENSOR_H
#define COAP_SENSOR_H
#include "packet.h"

class CoapSensor
{
public:
	CoapSensor()
	{
		this->set_method(GET|POST);
		this->set_name("unknown");
		this->set_fast(true);
		this->set_notify_time(20);
		this->set_content_type(TEXT_PLAIN);
	}
	CoapSensor(String name)
	{
		this->set_method(GET|POST);
		this->set_name(name);
		this->set_fast(true);
		this->set_notify_time(20);
		this->set_content_type(TEXT_PLAIN);
	}

	void init(String name, bool fast, uint16_t notify_time, uint8_t content_type)
	{
		this->set_method(GET|POST);
		this->set_name(name);
		this->set_fast(true);
		this->set_notify_time(notify_time);
		this->set_content_type(content_type);
	}

	virtual coap_status_t callback( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries);
	virtual uint8_t get_method();
	virtual String get_name();
	virtual bool get_fast();
	virtual uint16_t get_notify_time();
	virtual uint8_t get_content_type();
	virtual uint8_t set_method(uint8_t method);
	virtual String set_name(String name);
	virtual bool set_fast(bool fast);
	virtual uint16_t set_notify_time(uint16_t notify_time);
	virtual uint8_t set_content_type(uint8_t content_type);
	virtual void get_value(uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len);
	virtual void set_value(uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len);
private:
	String name;
	bool fast;
	uint16_t notify_time;
	uint8_t content_type, method;
	uint8_t enable_method(uint8_t method)
	{
		uint8_t tempmethod = this->get_method();
		tempmethod = tempmethod | method;
		this->set_method(tempmethod);
	}
	uint8_t disable_method(uint8_t method)
	{
		uint8_t tempmethod = this->get_method();
		tempmethod = tempmethod & (~method);
		this->set_method(tempmethod);
	}
};

#endif
