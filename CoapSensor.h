#ifndef COAP_SENSOR_H
#define COAP_SENSOR_H
#include "packet.h"

class CoapSensor
{
public:
	CoapSensor()
	{
		this->method = GET|POST;
		this->name = "unknown";
		this->fast = true;
		this->notify_time = 20;
		this->content_type = TEXT_PLAIN;
		this->changed = false;
	}
	CoapSensor(String name)
	{
		this->method = GET|POST;
		this->name = name;
		this->fast = true;
		this->notify_time = 20;
		this->content_type = TEXT_PLAIN;
		this->changed = false;
	}
	coap_status_t callback( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries);
	uint8_t get_method();
	String get_name();
	bool get_fast();
	bool is_changed();
	void mark_notified();
	uint16_t get_notify_time();
	uint8_t get_content_type();
	uint8_t set_method(uint8_t method);
	String set_name(String name);
	bool set_fast(bool fast);
	uint16_t set_notify_time(uint16_t notify_time);
	uint8_t set_content_type(uint8_t content_type);
	virtual void check(void);
	virtual void get_value(uint8_t* output_data, size_t* output_data_len);
	virtual void set_value(uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len);
private:
	String name;
	bool fast;
	bool changed;
	uint16_t notify_time;
	uint8_t content_type, method;
	uint8_t enable_method(uint8_t method);
	uint8_t disable_method(uint8_t method);
};

#endif
