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

#include <coap.h>

void Coap::init(uint16_t myAddress, BaseRouting * routing) {
    this->myAddress = myAddress;
    rcount = 0;
    last_broadcast = millis();
    routing_ = routing;

    broadcasting = true;
    mid_ = 1;
    observe_counter_ = 1;

#ifdef ENABLE_OBSERVE
    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {
        observers[i].observe_resource_ = NULL;
    }
#endif
}

void Coap::handler() {
#ifdef ENABLE_OBSERVE
    // notify observers
    coap_notify();
#endif

    // broadcast every 1000ms
    if (millis() - last_broadcast > 30 * 1000) {
        digitalWrite(13, HIGH);
        last_broadcast = millis();
        hereiam[0] = 'h';
        hereiam[1] = 'e';
        hereiam[2] = 'r';
        hereiam[3] = 'e';
        hereiam[4] = 'i';
        hereiam[5] = 'a';
        hereiam[6] = 'm';
        routing_->send(0xffff,(uint8_t*) hereiam, 7);
        digitalWrite(13, LOW);
    }

}

void Coap::add_resource(CoapSensor * sensor) {
    // remove if this resource is already stored (if we need to update)
    // remove_resource( name );
    // create new resource object
    size_t output_data_len;
    sensor->get_value(output_data, &output_data_len);
    sensor->set_value(output_data, 1, output_data, &output_data_len);
    resources_[rcount++] = resource_t(sensor);
}

void Coap::update_resource(char * name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type) {
    // TODO
}

void Coap::remove_resource(char * name) {
    // TODO
}

resource_t Coap::resource(uint8_t resource_id) {
    return resources_[resource_id];
}

/**
 * Generates the body of the response to a new .well-known/core request message.
 */

coap_status_t Coap::resource_discovery(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries) {

    if (method == COAP_GET) {

	char * output = (char *) output_data;
         size_t i, index = 0;
         for (i = 0; i < rcount; i++) {
 
             strcpy(output + index, "<");
             index++;
             strncpy(output + index, resources_[i].name(), resources_[i].name_length());
             index += resources_[i].name_length();
 
             strcpy(output + index, ">,");
             index += 2;
 
         }

         output_data[index - 1] = '\0';
         // set output data len
         *output_data_len = index;
      // return status
        return CONTENT;
    }
}

/**
 * Handles new incoming messages from XBEE.
 */
void Coap::receiver(uint8_t* buf, uint16_t from, uint8_t len) {
    // used to identify if this packet is a CoAP packet (not a CoAP feature)
    if (buf[0] != WISELIB_MID_COAP) {
        return;
    }


    coap_status_t coap_error_code;
    coap_packet_t msg;
    coap_packet_t response;
    uint8_t resource_id = 0;

    size_t output_data_len = 0;
    msg.init();
    response.init();

    memset(output_data, 0, CONF_LARGE_BUF_LEN);


    // parse the message
    coap_error_code = msg.buffer_to_packet(len, buf, (char*) helperBuf_);
    if (msg.version_w() != COAP_VERSION) {
        coap_error_code = BAD_REQUEST;
    }
    if (coap_error_code == COAP_NO_ERROR) {

        uint16_t address = this->myAddress;

        // if URI_HOST is set and the HOST doesn't much this host, reject the message
        if ((msg.is_option(URI_HOST)) && (msg.uri_host_w() != address)) {
            return;
        }
        //empty msg, ack, or rst
        if (msg.code_w() == 0) {
            coap_unregister_con_msg(msg.mid_w(), 0);
#ifdef ENABLE_OBSERVE
            if (msg.type_w() == RST) {
                coap_remove_observer(msg.mid_w());
            }
#endif
            return; // nothing else to do
        }
        // message is a request
        if (msg.code_w() <= 4) // 1-4
        {
            switch (msg.type_w()) {
                case CON:
                    response.set_type(ACK);
                    response.set_mid(msg.mid_w());
                    break;
                case NON:
                    response.set_type(NON);
                    response.set_mid(msg.mid_w());
                    break;
                default:
                    // ACK or RST on a request. Not a valid coap message, ignore
                    return;
            }
            //DBG(mySerial_->println("REC::REQUEST"));
            //DBG(mySerial_->println(make_string(msg.uri_path_w(), msg.uri_path_len_w())));

            CoapResource* res = NULL;

            if (strncmp(msg.uri_path_w(), ".well-known/core", msg.uri_path_len_w()) == 0) {
                if (msg.isGET()) {
                    response.set_code(resource_discovery(msg.code_w(), msg.payload_w(), msg.payload_len_w(), output_data, &output_data_len, msg.uri_queries_w()));
                    // set the content type
                    response.set_option(CONTENT_TYPE);
                    response.set_content_type(APPLICATION_LINK_FORMAT);
                    // check for blockwise response
                    uint8_t offset = coap_blockwise_response(&msg, &response, (uint8_t**) & output_data, &output_data_len);
                    // set the payload and length
                    response.set_payload(output_data + offset);
                    response.set_payload_len(output_data_len);
                    //digitalWrite(2,HIGH);
                }// end of method is allowed
                else {
                    //DBG(mySerial_->println("REC::METHOD_NOT_ALLOWED"));
                    response.set_code(METHOD_NOT_ALLOWED);
                }
#ifdef REMOTE_RESET
            } else if (strncmp(msg.uri_path_w(), "reset", msg.uri_path_len_w()) == 0) {
                if (msg.isPOST()) {
                    wdt_disable();
                    wdt_enable(WDTO_30MS);
                    while (1);
                } else {
                    //DBG(mySerial_->println("REC::METHOD_NOT_ALLOWED"));
                    response.set_code(METHOD_NOT_ALLOWED);
                } // if( method_allowed )
#endif
            } else if ((res = find_resource(msg.uri_path_w(), msg.uri_path_len_w())) != NULL) {
                //DBG(mySerial_->println("REC::RESOURCE FOUND"));
                // check if the requested method is allowed on this resource
                if (res->method_allowed(msg.code_w())) {
                    //DBG(mySerial_->println("REC::METHOD_ALLOWED"));
                    // in case of slow reply send the ACK if this is needed
                    if (res->fast_resource() == false && response.type_w() == ACK) {
                        // send the ACK
                        coap_send(&response, from);
                        // init the response again
                        response.init();
                        response.set_type(CON);
                        response.set_mid(coap_new_mid());
                    }
                    // execute the resource and set the status to the response object
                    response.set_code(res->execute(msg.code_w(), msg.payload_w(), msg.payload_len_w(), output_data, &output_data_len, msg.uri_queries_w()));
                    // set the content type
                    response.set_option(CONTENT_TYPE);
                    response.set_content_type(res->content_type());
                    // check for blockwise response
                    uint8_t offset = coap_blockwise_response(&msg, &response, (uint8_t**) & output_data, &output_data_len);
                    // set the payload and length
                    response.set_payload(output_data + offset);
                    response.set_payload_len(output_data_len);

#ifdef ENABLE_OBSERVE
                    // if it is set, register the observer
                    if (msg.code_w() == COAP_GET && msg.is_option(OBSERVE) && res->notify_time_w() > 0 && msg.is_option(TOKEN)) {
                        if (coap_add_observer(&msg, &from, res) == 1) {
                            response.set_option(OBSERVE);
                            response.set_observe(observe_counter_);
                        }
                    } // end of add observer
#endif
                }// end of method is allowed
                else {
                    //DBG(mySerial_->println("REC::METHOD_NOT_ALLOWED"));
                    response.set_code(METHOD_NOT_ALLOWED);
                } // if( method_allowed )
            }// end of resource found
            else {
                //DBG(mySerial_->println("REC::NOT_FOUND"));
                response.set_code(NOT_FOUND);
            }
            // if the request has a token, add it to the response
            if (msg.is_option(TOKEN)) {
                //DBG(mySerial_->println("REC::IS_SET_TOKEN"));
                response.set_option(TOKEN);
                response.set_token_len(msg.token_len_w());
                response.set_token(msg.token_w());
            }
            // send the reposnse
            coap_send(&response, from);
            //DBG(mySerial_->println("ACTION: Sent reply"));
            return;
        } // end of handle request
        // handle response
        if (msg.code_w() >= 64 && msg.code_w() <= 191) {
            //DBG(mySerial_->println("REC::RESPONSE"));
            switch (msg.type_w()) {
                case CON:
                    response.set_type(ACK);
                    response.set_mid(msg.mid_w());
                    coap_send(&response, from);
                    //DBG(mySerial_->println("ACTION: Sent ACK"));
                    break;
                case ACK:
                    coap_unregister_con_msg(msg.mid_w(), 0);
                    break;
                case RST:
#ifdef ENABLE_OBSERVE
                    coap_remove_observer(msg.mid_w());
#endif
                    coap_unregister_con_msg(msg.mid_w(), 0);
                    break;
            }
            return;
        }
    }// end of no error found
    else {
        // error found
        response.set_code(coap_error_code);
        if (msg.type_w() == CON) {
            response.set_type(ACK);
            response.set_mid(msg.mid_w());
        } else
            response.set_type(NON);
        coap_send(&response, from);
        //DBG(mySerial_->println("ACTION: Sent reply"));
    }
} // end of coap receiver

void Coap::coap_send(coap_packet_t *msg, uint16_t dest) {
    memset(sendBuf_, 0, CONF_MAX_MSG_LEN);
    uint8_t data_len = msg->packet_to_buffer(sendBuf_);
    if ((msg->type_w() == CON)) {
        coap_register_con_msg(dest, msg->mid_w(), sendBuf_, data_len, 0);
    }
    routing_->send(0xffff,sendBuf_, data_len);
    //    xbee_->send(tx_, 112);
}

uint16_t Coap::coap_new_mid() {
    return mid_++;
}

CoapResource* Coap::find_resource(char * uri_path, size_t len) {
    for (uint8_t i = 0; i < rcount; i++) {
        //DBG(mySerial_->println(resources_[*i].name()));
        if (strncmp(uri_path, resources_[i].name(), len) == 0) {
            return &(resources_[i]);
        }
    }
    return NULL;
}

uint8_t Coap::coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len) {
    //check if request is block	
    if (req->is_option(BLOCK2)) {
        if (req->block2_size_w() > CONF_MAX_PAYLOAD_LEN) {
            resp->set_block2_size(CONF_MAX_PAYLOAD_LEN);
            resp->set_block2_num(req->block2_num_w() * req->block2_size_w() / CONF_MAX_PAYLOAD_LEN);
        } else {
            resp->set_block2_size(req->block2_size_w());
            resp->set_block2_num(req->block2_num_w());
        }
        if (*data_len < resp->block2_size_w()) {
            resp->set_block2_more(0);
        } else if ((*data_len - req->block2_offset_w()) > resp->block2_size_w()) {
            resp->set_block2_more(1);
            *data_len = resp->block2_size_w();
        } else {
            resp->set_block2_more(0);
            *data_len -= req->block2_offset_w();
        }
        resp->set_option(BLOCK2);
        //*data += req->block2_offset_w();
        return req->block2_offset_w();
    }
    //check if the message needs to be blockwise
    if (*data_len > CONF_MAX_PAYLOAD_LEN) {
        resp->set_option(BLOCK2);
        resp->set_block2_num(0);
        resp->set_block2_more(1);
        resp->set_block2_size(CONF_MAX_PAYLOAD_LEN);
        *data_len = CONF_MAX_PAYLOAD_LEN;

    }
    return 0;
}

void Coap::coap_register_con_msg(uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries) {
    uint8_t i = 0;
    while (i < CONF_MAX_RETRANSMIT_SLOTS) {
        if (retransmit_mid_[i] == 0) {
            retransmit_register_[i] = 1;
            retransmit_id_[i] = id;
            retransmit_mid_[i] = mid;
            retransmit_timeout_and_tries_[i] = (CONF_COAP_RESPONSE_TIMEOUT << 4) | tries;
            retransmit_size_[i] = size;
            memcpy(retransmit_packet_[i], buf, size);
            // ARDUINO
            timeout_ = 1000 * (retransmit_timeout_and_tries_[i] >> 4);
            retransmit_timestamp_[i] = millis() + timeout_;
            return;
        }
        i++;
    }
}

uint8_t Coap::coap_unregister_con_msg(uint16_t mid, uint8_t flag) {
    uint8_t i = 0;
    while (i < CONF_MAX_RETRANSMIT_SLOTS) {
        if (retransmit_mid_[i] == mid) {
            if (flag == 1) {
                retransmit_register_[i] = 0;
                retransmit_id_[i] = 0x0000;
                retransmit_mid_[i] = 0x0000;
                memset(retransmit_packet_[i], 0, retransmit_size_[i]);
                retransmit_size_[i] = 0x00;
                retransmit_timeout_and_tries_[i] = 0x00;
                return 0;
            } else {
                retransmit_register_[i] = 0;
                return 0x0F & retransmit_timeout_and_tries_[i];
            }
        }
        i++;
    }
    return 0;
}

void Coap::coap_retransmit_loop(void) {
    //DBG(mySerial_->println("Retransmit loop"));
    uint8_t i;
    uint8_t timeout_factor = 0x01;
    for (i = 0; i < CONF_MAX_RETRANSMIT_SLOTS; i++) {
        //DBG(mySerial_->println(retransmit_register_[i]));
        if (retransmit_register_[i] == 1) {
            // -60 is used because there is always a fault in time
            if (retransmit_timestamp_[i] <= millis() - 60) {
                retransmit_timeout_and_tries_[i] += 1;
                timeout_factor = timeout_factor << (0x0F & retransmit_timeout_and_tries_[i]);
                // ARDUINO
                routing_->send(0xffff,retransmit_packet_[i], retransmit_size_[i]);
                //                xbee_->send(tx_, 112);

                if ((0x0F & retransmit_timeout_and_tries_[i]) == CONF_COAP_MAX_RETRANSMIT_TRIES) {
#ifdef ENABLE_OBSERVE
                    coap_remove_observer(retransmit_mid_[i]);
#endif
                    coap_unregister_con_msg(retransmit_mid_[i], 1);
                    return;
                } else {
                    // ARDUINO
                    timeout_ = timeout_factor * 1000 * (retransmit_timeout_and_tries_[i] >> 4);
                    retransmit_timestamp_[i] = millis() + timeout_;
                    return;
                }
            }
        }
    }
}

#ifdef ENABLE_OBSERVE

uint8_t Coap::coap_add_observer(coap_packet_t *msg, uint16_t *id, CoapResource* resource) {

    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {

        if ((observers[i].observe_id_ == *id) && (observers[i].observe_resource_ == resource)) {
            //update token
            memset(observers[i].observe_token_, 0, observers[i].observe_token_len_);
            observers[i].observe_token_len_ = msg->token_len_w();
            memcpy(observers[i].observe_token_, msg->token_w(), msg->token_len_w());
            return 1;
        }
        if (observers[i].observe_id_ == 0) {
            observers[i].observe_id_ = *id;
            observers[i].observe_token_len_ = msg->token_len_w();
            memcpy(observers[i].observe_token_, msg->token_w(), msg->token_len_w());
            observers[i].observe_resource_ = resource;
            observers[i].observe_last_mid_ = msg->mid_w();
            // ARDUINO
            //		  observers[i].observe_timestamp_ = millis() + 1000*resources_[resource].notify_time_w();
            observers[i].observe_timestamp_ = millis() + 1000;
            observe_counter_++;
            return 1;
        }
    }



    return 0;
}

bool Coap::coap_has_observers() {

    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {

        if (observers[i].observe_id_ != 0) {
            return true;
        }
    }
    return false;
}

void Coap::coap_remove_observer(uint16_t mid) {
    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {
        if (observers[i].observe_last_mid_ == mid) {
            //observers[i].observe_last_mid_ = 0;
            observers[i].observe_id_ = 0;
            //observers[i].observe_resource_ = NULL;
            //memset(observers[i].observe_token_, 0, observers[i].observe_token_len_);
            //observers[i].observe_token_len_ = 0;
            //observers[i].observe_timestamp_ = 0;
        }
    }
}

void Coap::coap_notify() {
    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {

        if (observers[i].observe_id_ == 0) continue;

        observer_t * observer = &(observers[i]);
        CoapResource* resource = observer->observe_resource_;

        if ((observer->observe_timestamp_ < millis()) || (resource->is_changed())) {
            coap_packet_t notification;
            uint8_t notification_size;
            //uint8_t output_data[CONF_LARGE_BUF_LEN];
            size_t output_data_len;
            memset(sendBuf_, 0, CONF_MAX_MSG_LEN);
            observer->observe_timestamp_ = millis() + resource->notify_time_w()*1000;

            // send msg
            notification.init();
            notification.set_type(CON);
            notification.set_mid(coap_new_mid());

            notification.set_code(resource->execute(COAP_GET, NULL, 0, output_data, &output_data_len, notification.uri_queries_w()));
            notification.set_option(CONTENT_TYPE);
            notification.set_content_type(resource->content_type());
            notification.set_option(TOKEN);
            notification.set_token_len(observer->observe_token_len_);
            notification.set_token(observer->observe_token_);
            notification.set_option(OBSERVE);
            //next notification will have greater observe option
            notification.set_observe(observe_counter_++);

            notification.set_payload(output_data);
            notification.set_payload_len(output_data_len);
            notification_size = notification.packet_to_buffer(sendBuf_);
            coap_register_con_msg(observers[i].observe_id_, notification.mid_w(), sendBuf_, notification_size, coap_unregister_con_msg(observer->observe_last_mid_, 0));
            observer->observe_last_mid_ = notification.mid_w();
            resource->mark_notified();

            // ARDUINO
            routing_->send(0xffff,sendBuf_, notification_size);
            //            xbee_->send(tx_, 112);
            break;
        }
    }

}
#endif

