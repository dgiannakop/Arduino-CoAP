#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include "vector.h"
#include "coap_conf.h"

typedef struct query_t
{
   String name;
   String value;
};
typedef query_t queries_t;

typedef enum
{
   CONTENT_TYPE = 1,
   MAX_AGE = 2,
   PROXY_URI = 3,
   ETAG = 4,
   URI_HOST = 5,
   LOCATION_PATH = 6,
   URI_PORT = 7,
   LOCATION_QUERY = 8,
   URI_PATH = 9,
   OBSERVE = 10,
   TOKEN = 11,
   ACCEPT = 12,
   IF_MATCH = 13,
   MAX_OFE = 14,
   URI_QUERY = 15,
   BLOCK2 = 17,
   BLOCK1 = 19,
   IF_NONE_MATCH = 21
} coap_option_t;

// non coap method values
typedef enum
{
   GET = 1,
   POST = 2,
   PUT = 4,
   DELETE = 8
} app_method_t;

// coap method values
typedef enum
{
   COAP_GET = 1,
   COAP_POST,
   COAP_PUT,
   COAP_DELETE
} coap_method_t;

typedef enum
{
   CON,
   NON,
   ACK,
   RST
} coap_message_type_t;

typedef enum
{
   TEXT_PLAIN = 0,
   TEXT_XML = 1,
   TEXT_CSV = 2,
   TEXT_HTML = 3,
   IMAGE_GIF = 21,
   IMAGE_JPEG = 22,
   IMAGE_PNG = 23,
   IMAGE_TIFF = 24,
   AUDIO_RAW = 25,
   VIDEO_RAW = 26,
   APPLICATION_LINK_FORMAT = 40,
   APPLICATION_XML = 41,
   APPLICATION_OCTET_STREAM = 42,
   APPLICATION_RDF_XML = 43,
   APPLICATION_SOAP_XML = 44,
   APPLICATION_ATOM_XML = 45,
   APPLICATION_XMPP_XML = 46,
   APPLICATION_EXI = 47,
   APPLICATION_FASTINFOSET = 48,
   APPLICATION_SOAP_FASTINFOSET = 49,
   APPLICATION_JSON = 50,
   APPLICATION_X_OBIX_BINARY = 51
} coap_content_type_t;

typedef enum
{
   COAP_NO_ERROR = 0,

   CREATED = 65,
   DELETED = 66,
   VALID = 67,
   CHANGED = 68,
   CONTENT = 69,

   BAD_REQUEST = 128,
   UNATHORIZED = 129,
   BAD_OPTION = 130,
   FORBIDDEN = 131,
   NOT_FOUND = 132,
   METHOD_NOT_ALLOWED = 133,
   PRECONDITION_FAILED = 140,
   REQUEST_ENTITY_TOO_LARGE = 141,
   UNSUPPORTED_MEDIA_TYPE = 143,

   INTERNAL_SERVER_ERROR = 160,
   NOT_IMPLEMENTED = 161,
   BAD_GATEWAY = 162,
   SERVICE_UNAVAILABLE = 163,
   GATEWAY_TIMEOUT = 164,
   PROXYING_NOT_SUPPORTED = 165
} coap_status_t;

class CoapPacket
{
   public:
      void init( void );
      coap_status_t buffer_to_packet( uint8_t len, uint8_t* buf, char* helperBuf );
      uint8_t packet_to_buffer( uint8_t* buf );
      // get
      uint8_t version_w();
      uint8_t type_w();
      uint8_t opt_count_w();
      uint8_t code_w();
      bool isGET();
      uint16_t mid_w();
      uint32_t is_option( uint8_t opt );
      uint8_t content_type_w();
      uint32_t max_age_w();
      uint16_t uri_host_w();
      uint16_t uri_port_w();
      uint8_t uri_path_len_w();
      char* uri_path_w();
      uint16_t observe_w();
      uint8_t token_len_w();
      uint8_t* token_w();
      uint16_t accept_w();
      queries_t uri_queries_w();
      uint32_t block2_num_w();
      uint8_t block2_more_w();
      uint16_t block2_size_w();
      uint32_t block2_offset_w();
      size_t payload_len_w();
      uint8_t* payload_w();
      // set
      void set_version( uint8_t version );
      void set_type( uint8_t type );
      void set_opt_count( uint8_t opt_count );
      void set_code( uint8_t code );
      void set_mid( uint16_t mid );
      void set_option( uint8_t opt );
      void set_content_type( uint8_t content_type );
      void set_max_age( uint32_t max_age );
      void set_uri_host( uint16_t uri_host );
      void set_uri_port( uint16_t uri_port );
      void set_uri_path_len( uint8_t uri_path_len );
      void set_uri_path( char* uri_path );
      void set_observe( uint16_t observe );
      void set_token_len( uint8_t token_len );
      void set_token( uint8_t* token );
      void set_accept( uint16_t accept );
      void set_uri_query( String uri_query_name, String uri_query_value );
      void set_block2_num( uint32_t block2_num );
      void set_block2_more( uint8_t block2_more );
      void set_block2_size( uint16_t block2_size );
      void set_block2_offset( uint32_t block2_offset );
      void set_payload_len( uint8_t payload_len );
      void set_payload( uint8_t* payload );
   protected:
      uint8_t add_fence_opt( uint8_t opt, uint8_t *current_delta, uint8_t *buf );
      uint8_t set_opt_header( uint8_t delta, size_t len, uint8_t *buf );
      uint8_t set_int_opt_value( uint8_t opt, uint8_t current_delta, uint8_t *buf, uint32_t value );
      uint32_t get_int_opt_value( uint8_t *value, uint16_t length );
      void merge_options( char **dst, size_t *dst_len, uint8_t *value, uint16_t length, char seperator );
      void make_uri_query( uint8_t* value, uint16_t length, char* largeBuf );
      uint8_t split_option( uint8_t opt, uint8_t current_delta, uint8_t* buf, char* seperator );
      uint8_t power_of_two( uint16_t num );
      String make_string( char* charArray, size_t charLen, char* largeBuf );
   private:
      uint8_t version_;
      uint8_t type_;
      uint8_t opt_count_;
      uint8_t code_;
      uint16_t mid_;

      uint32_t options_;

      uint8_t content_type_; // 1
      uint32_t max_age_; // 2
      //TODO...
      //size_t proxy_uri_len_; // 3
      //char *proxy_uri_; // 3
      //uint8_t etag_len_; // 4
      //uint8_t etag[8]_; // 4
      //size_t uri_host_len_; // 5
      uint16_t uri_host_; // 5
      uint16_t uri_port_; // 7
      //TODO...
      size_t uri_path_len_; // 9
      char *uri_path_; // 9
      uint16_t observe_; // 10
      uint8_t token_len_; // 11
      uint8_t token_[8]; // 11
      uint16_t accept_; // 12
      //TODO...
      //uint8_t if_match_len_; // 13
      //uint8_t if_match_[8]; // 13
      queries_t queries_; // 15

      // block2 17
      uint32_t block2_num_; // 17
      uint8_t block2_more_; // 17
      uint16_t block2_size_; // 17
      uint32_t block2_offset_; // 17
      //uint32_t block1_num_; // 19
      //uint8_t block1_more_; // 19
      //uint16_t block1_size_; // 19
      //uint32_t block1_offset_; // 19
      //uint8_t if_none_match; // 21

      uint8_t payload_len_;
      uint8_t *payload_;
};
#endif
