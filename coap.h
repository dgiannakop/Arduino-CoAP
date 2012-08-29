#ifndef coap_h
#define coap_h

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "vector.h"
#include "FastDelegate.h"
#include "coap_conf.h"
#include "packet.h"
#include "resource.h"

typedef CoapPacket coap_packet_t;
typedef CoapResource resource_t;
/*
typedef struct request_response_t
{
   coap_packet_t request;
   coap_packet_t response;
   uint8_t output_data[CONF_LARGE_BUF_LEN];
   size_t output_data_len;
};
typedef Vector<request_response_t> active_requests_t;
*/

class Coap {
	public:
#ifdef DEBUG
		void init(SoftwareSerial *mySerial, EthernetClass *ethernet, EthernetUDP *ethudp,
				  resource_t *resources, uint8_t *buf, char *largeBuf);
#else
		void init(EthernetClass *ethernet, EthernetUDP *ethudp);
#endif
		void handler(void);
		void add_resource(String name, uint8_t methods, my_delegate_t callback,
						  bool fast_resource, uint16_t notify_time, uint8_t content_type);
		void update_resource(String name, uint8_t methods, bool fast_resource,
							 int notify_time, uint8_t content_type);
		void remove_resource(String name);
		resource_t resource(uint8_t resource_id);
		coap_status_t resource_discovery(uint8_t method, uint8_t *input_data, size_t input_data_len,
										 uint8_t *output_data, size_t *output_len, queries_t queries);
		void receiver(uint8_t*, IPAddress, uint8_t);
		void coap_send(coap_packet_t*, IPAddress);
		uint16_t coap_new_mid();
		bool find_resource(uint8_t *i, String uri_path);
		//coap_status_t call_resource(uint8_t method, uint8_t resource_id, uint8_t* input_data,
		//							  size_t input_data_len, uint8_t* output_data, size_t* output_data_len,
		//							  queries_t queries );
		void coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len);
		void coap_register_con_msg(uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries);
		uint8_t coap_unregister_con_msg(uint16_t mid, uint8_t flag);
		void coap_retransmit_loop();
		//void coap_resource_discovery( size_t *payload_len );
#ifdef OBSERVING
		uint8_t coap_add_observer(coap_packet_t *msg, IPAddress* id, uint8_t resource_id);
		void coap_remove_observer(uint16_t mid);
		void coap_notify_from_timer();
		void coap_notify_from_interrupt(uint8_t resource_id);
		void coap_notify(uint8_t resource_id);
		uint16_t observe_counter();
		void increase_observe_counter();
#endif
		String make_string(char *charArray, size_t charLen);
		void debug_msg(uint8_t *msg, uint8_t len);
	private:
		EthernetClass* _ethernet;
		EthernetUDP*   _ethudp;
		/* Create the XbeeRadio object we'll be using */
		//XBeeRadio *xbee_;
		/* Create a reusable response object for responses we expect to handle */
		//XBeeRadioResponse *response_;
		/* Create a reusable rx16 response object to get the address */
		//Rx16Response *rx_;
		/* Create a tx16 request object */
		//Tx16Request tx_;
#ifdef DEBUG
		// Serial debug
		SoftwareSerial *mySerial_;
#endif
		//bool broadcasting;
		unsigned long timestamp;
		/* Message ID */
		uint16_t mid_;
		/* New vector type resources */
		Vector<CoapResource> resources_;
		/* Active requests vector */
		//active_requests_t active_requests_;
		//char *largeBuf_;
		/* Internal buffer for any reason */
		uint8_t* _helperBuffer; //CONF_HELPER_BUF_LEN
		/* Internal buffer for send */
		uint8_t* _sendBuffer; //CONF_MAX_MSG_LEN

		uint8_t* _packetBuffer; //UDP_TX_PACKET_MAX_SIZE

		/* _retransmit variables */
		unsigned long timeout_;
		struct retransmit_t {
			uint16_t id;
			uint16_t mid;
			uint8_t  reg;
			uint8_t  timeout_and_tries;
			uint8_t  size;
			uint8_t* packet; //CONF_MAX_MSG_LEN
			unsigned long timestamp;
		};
		retransmit_t* _retransmit; //CONF_MAX_RETRANSMIT_SLOTS

		uint16_t  observe_counter_;
		struct observe_t {
			uint8_t   id;
			IPAddress ip;
			uint8_t*  token; //8
			uint8_t   token_len;
			uint16_t  last_mid;
			uint8_t   resource;
			unsigned long timestamp;
		};
		observe_t* _observe; //CONF_MAX_OBSERVERS

#ifdef OBSERVING
		/* Observe variables */
#endif
};

#endif
