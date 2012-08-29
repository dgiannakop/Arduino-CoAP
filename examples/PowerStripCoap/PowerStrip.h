#ifndef PowerStrip_h
#define PowerStrip_h

#include <Arduino.h>
#include <coap.h>
#include <RelaySens.h>

// your app must have an init function where your resource is registered in coap
// callback functions must be char* function(uint8_t method)
// with method you can implement different operations for GET/PUT/POST/DELETE requests

class PowerStrip {
private:
	RelaySens plug[5];

	coap_status_t handleRelay(RelaySens* plug, uint8_t method, uint8_t *input_data, size_t input_len,
							  uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t handleSensor(RelaySens* plug, uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);

	Coap* _coap;

public:
	void init(Coap *coap);

	coap_status_t plug_sensor_0(uint8_t method, uint8_t *input_data, size_t input_len,
								uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_sensor_1(uint8_t method, uint8_t *input_data, size_t input_len,
								uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_sensor_2(uint8_t method, uint8_t *input_data, size_t input_len,
								uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_sensor_3(uint8_t method, uint8_t *input_data, size_t input_len,
								uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_sensor_4(uint8_t method, uint8_t *input_data, size_t input_len,
								uint8_t *output_data, size_t *output_len, queries_t queries);

	coap_status_t plug_relay_0(uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_relay_1(uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_relay_2(uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_relay_3(uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);
	coap_status_t plug_relay_4(uint8_t method, uint8_t *input_data, size_t input_len,
							   uint8_t *output_data, size_t *output_len, queries_t queries);
};

#endif
