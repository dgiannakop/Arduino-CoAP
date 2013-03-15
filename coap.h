#ifndef coap_h
#define coap_h

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "FastDelegate.h"
#include "coap_conf.h"
#include "packet.h"
#include "resource.h"
#include "CoapSensor.h"

#ifdef DEBUG
#define DBG(c) c
#define P(name) static const prog_uchar name[] PROGMEM
#else
#define DBG(c)
#endif

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

/*
 * Retransmit class
 */
struct retransmit_t {
    IPAddress ip;
    uint16_t port;

    uint16_t mid;
    uint8_t reg;
    uint8_t timeout_and_tries;
    uint8_t size;
    uint8_t packet[10];//[CONF_MAX_MSG_LEN/3]; //size: CONF_MAX_MSG_LEN
    unsigned long timestamp;
};

/*
 * Class used to hold observers
 */
struct observer_t {
    IPAddress ip;
    uint16_t port;

    uint8_t token[8]; //size: 8
    uint8_t token_len;
    uint16_t last_mid;
    CoapResource* resource;
    unsigned long timestamp;
};

/*
 * Coap class
 */
class Coap {
public:
    void init(EthernetClass *ethernet, EthernetUDP *ethudp);

    /**
     * Handles all requests.
     */
    void handler(void);
    /**
     * Checks all registered resources for changes.
     */
    void coap_check(void);

    void add_resource(CoapSensor * sensor);
    void update_resource(String name, uint8_t methods, bool fast_resource,
            int notify_time, uint8_t content_type);
    void remove_resource(String name);
    resource_t resource(uint8_t resource_id);
    coap_status_t resource_discovery(uint8_t method, uint8_t *input_data, size_t input_data_len,
            uint8_t *output_data, size_t *output_len, queries_t queries);
    void receiver(uint8_t*, IPAddress, uint16_t, uint8_t);
    void udp_send(IPAddress ip, uint16_t port, const uint8_t* buffer, size_t size);
    void coap_send(coap_packet_t*, IPAddress, uint16_t);
    uint16_t coap_new_mid();
    CoapResource* find_resource(String uri_path);

    //coap_status_t call_resource(uint8_t method, uint8_t resource_id, uint8_t* input_data,
    //							  size_t input_data_len, uint8_t* output_data, size_t* output_data_len,
    //							  queries_t queries );
    int coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data,
            size_t *data_len);

    uint8_t coap_register_con_msg(IPAddress id, uint16_t port, uint16_t mid, uint8_t *buf,
            uint8_t size, uint8_t tries);
    uint8_t coap_unregister_con_msg(uint16_t mid, uint8_t flag);
    void coap_retransmit_loop();
    //void coap_resource_discovery( size_t *payload_len );
#ifdef ENABLE_OBSERVE
    uint8_t coap_add_observer(coap_packet_t *msg, IPAddress* id, uint16_t, CoapResource * sensor);
    void coap_remove_observer(uint16_t mid);
    void coap_notify_from_interrupt(uint8_t resource_id);
    void coap_notify();
    //		uint8_t get_observer_counter();
    //		void    inc_observer_counter();
#endif
    String make_string(char *charArray, size_t charLen);
    void debug_msg(uint8_t *msg, uint8_t len);
private:
    EthernetClass* _ethernet;
    EthernetUDP* _ethudp;

    //bool broadcasting;
    unsigned long _timestamp;
    /* Message ID */
    uint16_t _mid;
    /* Active requests vector */
    //active_requests_t active_requests_;
    //char *largeBuf_;
    /* Internal buffer for any reason */
    uint8_t _helper_buffer[CONF_HELPER_BUF_LEN];
    /* Internal buffer for send */
    uint8_t _send_buffer[CONF_MAX_MSG_LEN];
    /* buffer used by the arduino's' udp implementation */
    uint8_t _packet_buffer[UDP_TX_PACKET_MAX_SIZE];

    uint8_t _large_buffer[CONF_LARGE_BUF_LEN];

    /* resource variables */
    uint8_t _resource_counter;
    resource_t _resource[CONF_MAX_RESOURCES];
    observer_t _observer[CONF_MAX_OBSERVERS];
    //resource_t** _resource; // size: CONF_MAX_RESOURCES
    resource_t* allocateResourceSlot(); // unused
    void freeResourceSlot(resource_t*); // unused

    /* retransmit variables */
    unsigned long _timeout;
    uint8_t _retransmit_slot_counter;
    retransmit_t _retransmit[CONF_MAX_RETRANSMIT_SLOTS]; // size: CONF_MAX_RETRANSMIT_SLOTS
    /* retransmit functions */
    retransmit_t* allocateRetransmitSlot(int size);
    int freeRetransmitSlot(uint8_t mid);

    /* observer variables */
    uint16_t _observe_counter;
    uint8_t _observer_slot_counter;
    /* observer functions */
    observer_t* allocateObserverSlot();
    void freeObserverSlot(int mid);

#ifdef ENABLE_OBSERVE
    /* Observe variables */
#endif
};
#endif
