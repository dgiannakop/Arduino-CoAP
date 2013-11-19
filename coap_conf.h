/********************************************************************************
 ** The Arduino-CoAP is free software: you can redistribute it and/or modify   **
 ** it under the terms of the GNU Lesser General Public License as             **
 ** published by the Free Software Foundation, either version 3 of the         **
 ** License, or (at your option) any later version.                            **
 **                                                                            **
 ** The Arduino-CoAP is distributed in the hope that it will be useful,        **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of             **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              **
 ** GNU Lesser General Public License for more details.                        **
 **                                                                            **
 ** You should have received a copy of the GNU Lesser General Public           **
 ** License along with the Arduino-CoAP.                                       **
 ** If not, see <http://www.gnu.org/licenses/>.                                **
 *******************************************************************************/

#define WISELIB_MID_COAP                    51
// end of wiselib defines
// CONFIGURATION
#define CONF_COAP_RESOURCE_NAME_SIZE        7
#define CONF_MAX_RESOURCES                  9
#define CONF_LARGE_BUF_LEN                  55
#define CONF_HELPER_BUF_LEN                 CONF_LARGE_BUF_LEN
#define CONF_MAX_RESOURCE_QUERIES           1
#define CONF_MAX_OBSERVERS                  CONF_MAX_RESOURCES
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
