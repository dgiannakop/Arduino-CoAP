#ifndef COAPTYPEDEF_H
#define COAPTYPEDEF_H

#include <Arduino.h>
typedef struct query_t {
    String name;
    String value;
};
typedef query_t queries_t;

typedef enum {
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

typedef enum {
    GET = 1,
    POST = 2,
    PUT = 4,
    DELETE = 8
} app_method_t;

// coap method values

typedef enum {
    COAP_GET = 1,
    COAP_POST,
    COAP_PUT,
    COAP_DELETE
} coap_method_t;

typedef enum {
    CON,
    NON,
    ACK,
    RST
} coap_message_type_t;

typedef enum {
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

typedef enum {
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

#endif