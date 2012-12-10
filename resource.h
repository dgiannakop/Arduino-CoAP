#ifndef RESOURCE_H
#define RESOURCE_H

#include <Arduino.h>
#include "packet.h"
#include "CoapSensor.h"

class CoapResource {
public:

    CoapResource();
    /**
     * Resource Constructor. User to pass as parameters all required fields: 
     *
     */
    CoapResource(CoapSensor * sensor);
    /**
     * Called when a resouce is to be evaluated again.
     */
    coap_status_t execute(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries);
    /**
     * Sets a new notification interval for the current resource.
     */
    void set_notify_time(uint16_t notify_time);
    /**
     * True if sensor value has changed.
     */
    bool is_changed();
    void mark_notified();
    /**
     * 
     */
    bool is_set();
    /**
     * String representation of the resource's name.
     */
    String name();
    /**
     * Length of the resource's name.
     */
    uint8_t name_length();
    void nameToStr(char* buf, size_t len);
    /**
     * Check if the resource allows the method.
     */
    uint8_t method_allowed(uint8_t method);

    uint16_t notify_time_w();
    uint8_t resource_len();
    bool fast_resource();
    uint8_t content_type();
    bool interrupt_flag_w();
    void check();

private:
    bool is_set_;
    CoapSensor *del_;
    bool interrupt_flag_;
};
#endif

