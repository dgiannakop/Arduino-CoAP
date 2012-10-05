#define WISELIB_MID_COAP                    51
// end of wiselib defines
// CONFIGURATION
#define CONF_LARGE_BUF_LEN                  100
#define CONF_HELPER_BUF_LEN                 64
#define CONF_MAX_RESOURCES                  10
#define CONF_MAX_RESOURCE_QUERIES           1
#define CONF_MAX_OBSERVERS                  9
#define CONF_MAX_MSG_LEN                    99
#define CONF_MAX_PAYLOAD_LEN                64
#define CONF_PIGGY_BACKED                   1
#define CONF_MAX_RETRANSMIT_SLOTS           2

#define CONF_COAP_RESPONSE_TIMEOUT          2
#define CONF_COAP_RESPONSE_RANDOM_FACTOR    1.5
#define CONF_COAP_MAX_RETRANSMIT_TRIES      4
// END OF CONFIGURATION

#include "coap_attributes.h"

// CONFIGURE  OPERTATIONS
//#define ENABLE_DEBUG
#define ENABLE_OBSERVE
