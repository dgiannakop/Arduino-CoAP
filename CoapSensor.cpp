#include "CoapSensor.h"
uint8_t CoapSensor::get_method()
{
	return method;
}

String CoapSensor::get_name()
{
	return name;
}

bool CoapSensor::get_fast()
{
	return fast;
}

uint16_t CoapSensor::get_notify_time()
{
	return notify_time;
}

uint8_t CoapSensor::get_content_type()
{
	return content_type;
}
uint8_t CoapSensor::set_method(uint8_t method)
{
	this->method = method;
}

String CoapSensor::set_name(String name)
{
	this->name = name;
}

bool CoapSensor::set_fast(bool fast)
{
	this->fast = fast;
}

uint16_t CoapSensor::set_notify_time(uint16_t notify_time)
{
	this->notify_time = notify_time;
}

uint8_t CoapSensor::set_content_type(uint8_t content_type)
{
	this->content_type = content_type;
}


coap_status_t CoapSensor::callback( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries)
{
	char bla[] = "default";
	memcpy(output_data, bla, strlen(bla)+1);
	*output_data_len = strlen(bla)+1;
	if( method == GET )
	{
		this->get_value(output_data, output_data_len);
		return CONTENT;
	}
	else if (method==POST)
	{
		this->set_value(input_data, input_data_len, output_data, output_data_len);
		return CHANGED;      
	}
}

void CoapSensor::get_value(uint8_t* output_data, size_t* output_data_len)
{
	this->disable_method(GET);
}

void  CoapSensor::set_value(uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len)
{
	this->disable_method(POST);
}

void CoapSensor::check(void){}

uint8_t CoapSensor::enable_method(uint8_t method)
{
	uint8_t tempmethod = this->get_method();
	tempmethod = tempmethod | method;
	this->set_method(tempmethod);
}
uint8_t CoapSensor::disable_method(uint8_t method)
{
	uint8_t tempmethod = this->get_method();
	tempmethod = tempmethod & (~method);
	this->set_method(tempmethod);
}
bool CoapSensor::is_changed()
{
	return changed;
}

void CoapSensor::mark_notified(){
	changed=false;
}
