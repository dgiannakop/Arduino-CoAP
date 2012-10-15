#ifndef coap_h
#define coap_h

#include <Arduino.h>
#include <XbeeRadio.h>
#include "coap_conf.h"
//#include "vector.h"
#include "packet.h"
#include "CoapSensor.h"



#include "resource.h"



typedef CoapPacket coap_packet_t;
typedef CoapResource resource_t;

typedef struct observer_t {
	uint16_t observe_id_;      
	uint8_t observe_token_len_;
	uint16_t observe_last_mid_;
	CoapResource* observe_resource_;
	uint8_t observe_token_[8];
	unsigned long observe_timestamp_;
} ;

//typedef Vector<resource_t> vector_t;


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
class Coap
{
   public:
#ifdef ENABLE_DEBUG
      void init( SoftwareSerial *mySerial, XBeeRadio* xbee, XBeeRadioResponse* response, Rx16Response* rx, resource_t* resources, uint8_t* buf, char* largeBuf );
#else
      void init( XBeeRadio* xbee, XBeeRadioResponse* response, Rx16Response* rx );
#endif
      void handler( void );
      void add_resource( CoapSensor * sensor );
      void update_resource( String name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type );
      void remove_resource( String name );
      resource_t resource( uint8_t resource_id );
      
      /**
       * Generates the body of the response to a new .well-known/core request message.
       */
      coap_status_t resource_discovery( uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries );
	/**
	* Handles new incoming messages from XBEE.
	*/
      void receiver( uint8_t*, uint16_t, uint8_t );
      void coap_send( coap_packet_t*, uint16_t );
      uint16_t coap_new_mid();
      CoapResource* find_resource(  String uri_path );
      //coap_status_t call_resource( uint8_t method, uint8_t resource_id, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries );
      int coap_blockwise_response( coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len );
      void coap_register_con_msg( uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries );
      uint8_t coap_unregister_con_msg( uint16_t mid, uint8_t flag );
      void coap_retransmit_loop();
      //void coap_resource_discovery( size_t *payload_len );
#ifdef ENABLE_OBSERVE
      uint8_t coap_add_observer( coap_packet_t *msg, uint16_t *id, CoapResource* resource_id );
      void coap_remove_observer( uint16_t mid );
      void coap_notify_from_timer( );
      void coap_notify_from_interrupt( uint8_t resource_id );
      void coap_notify( void );
      //uint16_t observe_counter();
      void increase_observe_counter();
#endif

      String make_string( char* charArray, size_t charLen );
      void debug_msg( uint8_t* msg, uint8_t len );
      

   private:
	  
      //Create the XbeeRadio object we'll be using
      XBeeRadio *xbee_;
      // create a reusable response object for responses we expect to handle
      XBeeRadioResponse *response_;
      // create a reusable rx16 response object to get the address
      Rx16Response *rx_;
      // create a tx16 request object
      Tx16Request tx_;
#ifdef ENABLE_DEBUG
      // Serial debug
      SoftwareSerial *mySerial_;
#endif
      bool broadcasting;
      unsigned long timestamp;
	  unsigned long last_broadcast;     
	  char hereiam[8];    
	  
      // Message ID
      uint16_t mid_;
      // new vector type resources
      
      // active requests vector
      //active_requests_t active_requests_;
      //char *largeBuf_;
      // Internal buffer for any reason
      uint8_t helperBuf_[CONF_HELPER_BUF_LEN];
      // Internal buffer for send
      uint8_t sendBuf_[CONF_MAX_MSG_LEN];
      // retransmit variables
      uint16_t retransmit_id_[CONF_MAX_RETRANSMIT_SLOTS];
      uint16_t retransmit_mid_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_register_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_timeout_and_tries_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_size_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_packet_[CONF_MAX_RETRANSMIT_SLOTS][CONF_MAX_MSG_LEN];
      unsigned long retransmit_timestamp_[CONF_MAX_RETRANSMIT_SLOTS];
      unsigned long timeout_;
      resource_t resources_[CONF_MAX_RESOURCES];
		int rcount;
#ifdef ENABLE_OBSERVE

		observer_t observers[CONF_MAX_OBSERVERS];
      //uint16_t observe_id_[CONF_MAX_OBSERVERS];
      //uint8_t observe_token_[CONF_MAX_OBSERVERS][8];
      //uint8_t observe_token_len_[CONF_MAX_OBSERVERS];
      //uint16_t observe_last_mid_[CONF_MAX_OBSERVERS];
      //uint8_t observe_resource_[CONF_MAX_OBSERVERS];
      //unsigned long observe_timestamp_[CONF_MAX_OBSERVERS];
      


      // observe variables

#endif
      uint16_t observe_counter_;
   uint8_t output_data[CONF_LARGE_BUF_LEN];
	
};

#endif
