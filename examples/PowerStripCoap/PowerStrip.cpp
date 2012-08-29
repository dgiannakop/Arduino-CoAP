#include "PowerStrip.h"

void PowerStrip::init(Coap *coap)
{
	_coap = coap;

	uint8_t SensorPin[] = {A0, A1, A2, A3, A4};
	uint8_t RelayPin[] = {2, 3, 4, 5, 6};

	my_delegate_t plug_sensor_d[5];
	my_delegate_t plug_relay_d[5];

	for(uint8_t i = 0; i < 2; i++) plug[i].init(RelayPin[i], SensorPin[i]);

	// first we create a delegate for our callback function
	plug_sensor_d[0] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_sensor_0);
	coap->add_resource("1/c", GET, plug_sensor_d[0], true, 20, TEXT_PLAIN);

	plug_sensor_d[1] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_sensor_1);
	coap->add_resource("2/c", GET, plug_sensor_d[1], true, 20, TEXT_PLAIN);

	plug_sensor_d[2] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_sensor_2);
	coap->add_resource("3/c", GET, plug_sensor_d[2], true, 20, TEXT_PLAIN);
//
	plug_sensor_d[3] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_sensor_3);
	coap->add_resource("4/c", GET, plug_sensor_d[3], true, 20, TEXT_PLAIN);
//
//	plug_sensor_d[4] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_sensor_4);
//	coap->add_resource("5/c", GET, plug_sensor_d[4], true, 20, TEXT_PLAIN);


	plug_relay_d[0] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_relay_0);
	coap->add_resource("1/s", GET|PUT, plug_relay_d[0], true, 20, TEXT_PLAIN);

	plug_relay_d[1] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_relay_1);
	coap->add_resource("2/s", GET|PUT, plug_relay_d[1], true, 20, TEXT_PLAIN);

	plug_relay_d[2] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_relay_2);
	coap->add_resource("3/s", GET|PUT, plug_relay_d[2], true, 20, TEXT_PLAIN);
//
	plug_relay_d[3] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_relay_3);
	coap->add_resource("4/s", GET|PUT, plug_relay_d[3], true, 20, TEXT_PLAIN);
//
//	plug_relay_d[4] = fastdelegate::MakeDelegate(this, &PowerStrip::plug_relay_4);
//	coap->add_resource("5/s", GET|PUT, plug_relay_d[4], true, 20, TEXT_PLAIN);
}


coap_status_t PowerStrip::handleRelay(RelaySens *plug, uint8_t method, uint8_t *input_data, size_t input_len,
							 uint8_t *output_data, size_t *output_len, queries_t queries)
{
	if(method == GET) {
		*output_len = sprintf((char *)output_data, "%d", plug->state);
		return CONTENT;
	} else if(method == PUT) {
		uint8_t input = *input_data - 0x30;
		if(input == 0 && plug->state == 1) plug->Relayclose();
		else if(input >= 1 && plug->state == 0) plug->Relayopen();
		*output_len = sprintf((char *)output_data, "%d", plug->state);
		return CHANGED;
	}
}

coap_status_t PowerStrip::plug_relay_0(uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleRelay(&plug[0], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_relay_1(uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleRelay(&plug[1], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_relay_2(uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleRelay(&plug[2], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_relay_3(uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleRelay(&plug[3], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_relay_4(uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleRelay(&plug[4], method, input_data, input_len, output_data, output_len, queries));
}


coap_status_t PowerStrip::handleSensor(RelaySens *plug, uint8_t method, uint8_t *input_data, size_t input_len,
									   uint8_t *output_data, size_t *output_len, queries_t queries)
{
	*output_len = sprintf((char *)output_data, "%dmA", plug->ReadSensor());
	return CONTENT;
}

coap_status_t PowerStrip::plug_sensor_0(uint8_t method, uint8_t *input_data, size_t input_len,
										uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleSensor(&plug[0], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_sensor_1(uint8_t method, uint8_t *input_data, size_t input_len,
										uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleSensor(&plug[1], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_sensor_2(uint8_t method, uint8_t *input_data, size_t input_len,
										uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleSensor(&plug[2], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_sensor_3(uint8_t method, uint8_t *input_data, size_t input_len,
										uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleSensor(&plug[3], method, input_data, input_len, output_data, output_len, queries));
}

coap_status_t PowerStrip::plug_sensor_4(uint8_t method, uint8_t *input_data, size_t input_len,
										uint8_t *output_data, size_t *output_len, queries_t queries)
{
	return(handleSensor(&plug[4], method, input_data, input_len, output_data, output_len, queries));
}

