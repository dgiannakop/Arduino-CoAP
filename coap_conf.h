#define WISELIB_MID_COAP                    51
// end of wiselib defines
// CONFIGURATION
#define CONF_LARGE_BUF_LEN                  128
#define CONF_HELPER_BUF_LEN                 64
#define CONF_MAX_RESOURCES                  2
#define CONF_MAX_RESOURCE_QUERIES           3
#define CONF_MAX_OBSERVERS                  2
#define CONF_MAX_MSG_LEN                    99
#define CONF_MAX_PAYLOAD_LEN                64
#define CONF_PIGGY_BACKED                   1
#define CONF_MAX_RETRANSMIT_SLOTS           2

#define CONF_COAP_RESPONSE_TIMEOUT          2
#define CONF_COAP_RESPONSE_RANDOM_FACTOR    1.5
#define CONF_COAP_MAX_RETRANSMIT_TRIES      4
// END OF CONFIGURATION
// CURRENT COAP DEFINES
#define COAP_VERSION                        1
#define COAP_HEADER_VERSION_MASK            0xC0
#define COAP_HEADER_VERSION_SHIFT           6
#define COAP_HEADER_TYPE_MASK               0x30
#define COAP_HEADER_TYPE_SHIFT              4
#define COAP_HEADER_OPT_COUNT_MASK          0x0F
#define COAP_HEADER_OPT_COUNT_SHIFT         0
#define COAP_HEADER_LEN                     4

//#define DEBUG
#define OBSERVING
