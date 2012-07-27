#include "App.h"

void App::init(Coap *coap)
{
	coap_ = coap;
	my_delegate_t delegate;
	my_delegate_t delegate2;
	my_delegate_t delegate3;

	lampPins[0] = 3;
	lampPins[1] = 4;
	lampPins[2] = 5;
	lampPins[3] = 6;

	lampStatuses[0] = 0;
	lampStatuses[1] = 0;
	lampStatuses[2] = 0;
	lampStatuses[3] = 0;

	ledState = 0;

	// first we create a delegate for our callback function
	delegate = fastdelegate::MakeDelegate(this, &App::test);
	coap->add_resource("lamps", GET | PUT, delegate, true, 20, TEXT_PLAIN);
	//delegate2 = fastdelegate::MakeDelegate( this, &App::test2 );
	//coap->add_resource("temp1", GET | PUT, delegate2, true, 30, APPLICATION_XML);
	//delegate3 = fastdelegate::MakeDelegate( this, &App::test3 );
	//coap->add_resource("msg", PUT, delegate3, true, 60, TEXT_PLAIN);
}

void App::setLamp(int lamp, int value)
{
	lampStatuses[lamp] = value;
	digitalWrite(lampPins[lamp], lampStatuses[lamp]);
}

coap_status_t App::test(uint8_t method, uint8_t *input_data, size_t input_data_len, uint8_t *output_data, size_t *output_data_len, queries_t queries)
{
	//*output_data_len = sprintf( (char*)output_data, "working" );
	//uint8_t* input = resources_[rid].input_data_w();
	if(method == GET) {
		*output_data_len = sprintf((char *)output_data, "%d", lampStatuses[0]);
		return CONTENT;
	} else if(method == PUT) {
		setLamp(0, *input_data - 0x30);
		*output_data_len = sprintf((char *)output_data, "%d", lampStatuses[0]);
		return CHANGED;
	}
}
coap_status_t App::test2(uint8_t method, uint8_t *input_data, size_t input_data_len, uint8_t *output_data, size_t *output_data_len, queries_t queries)
{
	*output_data_len = sprintf((char *)output_data, "working2");
	/*
	  char char_buffer[query_value.length()];
	 query_value.toCharArray(char_buffer, query_value.length());
	 int i = int(char_buffer);
	 coap_->update_resource("test", GET | PUT, true, i, TEXT_PLAIN);
	 //resources_[rid].set_payload_len( sprintf( data_, "working" ) );
	 */
	return CONTENT;
}
coap_status_t App::test3(uint8_t method, uint8_t *input_data, size_t input_data_len, uint8_t *output_data, size_t *output_data_len, queries_t queries)
{
	/*
	int i;
	 for(i=0; i<input_data_len; i++);
	 {
	 output_data[i] = input_data[i];
	 }
	 *output_data_len = input_data_len;
	 */
	*output_data_len = sprintf((char *)output_data, "%d", input_data_len);
	/*
	  char char_buffer[query_value.length()];
	 query_value.toCharArray(char_buffer, query_value.length());
	 int i = int(char_buffer);
	 coap_->update_resource("test", GET | PUT, true, i, TEXT_PLAIN);
	 //resources_[rid].set_payload_len( sprintf( data_, "working" ) );
	 */
	return CONTENT;
}

