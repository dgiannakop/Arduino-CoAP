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

#ifndef COAP_H
#define COAP_H

#include <Arduino.h>
#include <TreeRouting.h>
//Software Reset
#include <avr/wdt.h>
#include "coap_conf.h"
//#include "vector.h"
#include "packet.h"
#include "CoapSensor.h"

#include "resource.h"
#include "observer.h"



typedef CoapPacket coap_packet_t;
typedef CoapResource resource_t;

/**
 * The Coap Server Class.
 * 
 * @param xbee The XBEE to use for communication.
 * @param response A response instance to use for outgoing messages.
 * @param rx An Rx16Response instance for outgoing XBEE addresses.
 */
class Coap {
public:
#ifdef ENABLE_DEBUG
    void init(SoftwareSerial *mySerial, XBeeRadio* xbee, XBeeRadioResponse* response, Rx16Response* rx, resource_t* resources, uint8_t* buf, char* largeBuf);
#else
    void init(uint16_t myAddress, TreeRouting * routing);
#endif

    /**
     * Handles all requests.
     */
    void handler(void);
    /**
     * Checks all registered resources for changes.
     */
    void coap_check(void);
    /**
     * Adds a new Sensor to Coap.
     * @param a pointer to the new Sensor.
     */
    void add_resource(CoapSensor * sensor);
    /**
     * 
     * @param name
     * @param methods
     * @param fast_resource
     * @param notify_time
     * @param content_type
     */
    void update_resource(char * name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type);
    /**
     * Removes a resource from the CoAP server.
     * @param name the name of the resource to remove.
     */
    void remove_resource(char * name);
    resource_t resource(uint8_t resource_id);

    /**
     * Generates the body of the response to a new .well-known/core request message.
     * @param method
     * @param input_data
     * @param input_data_len
     * @param output_data
     * @param output_len
     * @param queries
     * @return 
     */
    coap_status_t resource_discovery(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_len, queries_t queries);
    /**
     * Handles new incoming messages from XBEE.
     * @param 
     * @param 
     * @param 
     */
    void receiver(uint8_t*, uint16_t, uint8_t);
    /**
     * Sends the given packet to the destination.
     * @param packet the message to send.
     * @param destination the destination of the message.
     */
    void coap_send(coap_packet_t* packet, uint16_t destination);
    /**
     * Get the next available MID for a new CoAP message.
     * @return the next MID.
     */
    uint16_t coap_new_mid();
    /**
     * 
     * @param uri_path
     * @return 
     */
    CoapResource* find_resource(char * uri_path, size_t len);
    /**
     * 
     * @param req
     * @param resp
     * @param data
     * @param data_len
     * @return 
     */
    int coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len);
    /**
     * 
     * @param id
     * @param mid
     * @param buf
     * @param size
     * @param tries
     */
    void coap_register_con_msg(uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries);
    /**
     * 
     * @param mid
     * @param flag
     * @return 
     */
    uint8_t coap_unregister_con_msg(uint16_t mid, uint8_t flag);
    /**
     * 
     */
    void coap_retransmit_loop();
    //coap_status_t call_resource( uint8_t method, uint8_t resource_id, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries );
    //void coap_resource_discovery( size_t *payload_len );
#ifdef ENABLE_OBSERVE    
    uint8_t coap_add_observer(coap_packet_t *msg, uint16_t *id, CoapResource* resource_id);
    void coap_remove_observer(uint16_t mid);
    void coap_notify_from_timer();
    void coap_notify_from_interrupt(uint8_t resource_id);
    void coap_notify(void);
    //uint16_t observe_counter();
    void increase_observe_counter();
    bool coap_has_observers();
#endif

    /**
     * 
     * @param msg
     * @param len
     */
    //String make_string(char* charArray, size_t charLen);
    /**
     * 
     * @param msg
     * @param len
     */
    void debug_msg(uint8_t* msg, uint8_t len);


private:

#ifdef ENABLE_DEBUG
    // Serial debug
    SoftwareSerial *mySerial_;
#endif
    bool broadcasting;
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
#endif
    // observe variables
    uint16_t observe_counter_;
    uint8_t output_data[CONF_LARGE_BUF_LEN];
    TreeRouting * routing_;
    uint16_t myAddress;
};

#endif
