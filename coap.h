#ifndef coap_h
#define coap_h

#include <Arduino.h>
#include "FastDelegate.h"
#include <XbeeRadio.h>
#include "coap_conf.h"
#include "vector.h"
#include "packet.h"
#include "resource.h"

typedef CoapPacket coap_packet_t;
typedef CoapResource resource_t;
typedef Vector<resource_t> vector_t;

class Coap
{
   public:
#ifdef DEBUG
      void init( /*SimpleTimer* timer,*/ SoftwareSerial *mySerial_, XBeeRadio* xbee, XBeeRadioResponse* response, Rx16Response* rx, resource_t* resources, uint8_t* buf, char* largeBuf );
#else
      void init( /*SimpleTimer* timer,*/ XBeeRadio* xbee, XBeeRadioResponse* response, Rx16Response* rx, uint8_t* buf, char* largeBuf );
#endif
      void handler( void );
      void add_resource(String name, uint8_t methods, my_delegate_t callback, bool fast_resource, uint16_t notify_time, uint8_t content_type);
      void update_resource(String name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type);
      void remove_resource(String name);
      resource_t resource(uint8_t resource_id);
      char* resource_discovery( uint8_t method, uint8_t* input_data, size_t input_data_len, size_t* output_len, queries_t queries );
      void receiver( uint8_t*, uint16_t, uint8_t );
      void coap_send( coap_packet_t*, uint16_t );
      uint16_t coap_new_mid();
      bool find_resource( uint8_t* i, String uri_path );
      coap_status_t call_resource( uint8_t method, uint8_t resource_id, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries );
      void coap_blockwise_response( coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len );
      void coap_register_con_msg( uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries );
      uint8_t coap_unregister_con_msg( uint16_t mid, uint8_t flag );
      void coap_retransmit_loop();
      void coap_resource_discovery( char* data, size_t *payload_len );
#ifdef OBSERVING
      uint8_t coap_add_observer( coap_packet_t *msg, uint16_t *id, uint8_t resource_id );
      void coap_remove_observer( uint16_t mid );
      void coap_notify_from_timer( );
      void coap_notify_from_interrupt( uint8_t resource_id );
      void coap_notify( uint8_t resource_id );
      uint16_t observe_counter();
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
      // timer for interupts
      //SimpleTimer *timer_;
#ifdef DEBUG
      // Serial debug
      SoftwareSerial *mySerial_;
#endif
      bool broadcasting;
      unsigned long timestamp;
      // Message ID
      uint16_t mid_;
      // Resources pointer
      //resource_t* resources_;
      // new vector type resources
      vector_t resources_;

      char *largeBuf_;
      // Internal buffer for messages
      uint8_t *buf_;
      // retransmit variables
      uint16_t retransmit_id_[CONF_MAX_RETRANSMIT_SLOTS];
      uint16_t retransmit_mid_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_register_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_timeout_and_tries_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_size_[CONF_MAX_RETRANSMIT_SLOTS];
      uint8_t retransmit_packet_[CONF_MAX_RETRANSMIT_SLOTS][CONF_MAX_MSG_LEN];
      unsigned long retransmit_timestamp_[CONF_MAX_RETRANSMIT_SLOTS];
      unsigned long timeout_;

      uint16_t observe_id_[CONF_MAX_OBSERVERS];
      uint8_t observe_token_[CONF_MAX_OBSERVERS][8];
      uint8_t observe_token_len_[CONF_MAX_OBSERVERS];
      uint16_t observe_last_mid_[CONF_MAX_OBSERVERS];
      uint8_t observe_resource_[CONF_MAX_OBSERVERS];
      uint16_t observe_counter_;
      unsigned long observe_timestamp_[CONF_MAX_OBSERVERS];
#ifdef OBSERVING
      // observe variables

#endif
};

#endif
