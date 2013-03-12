#include "coap.h"

void Coap::init(EthernetClass* ethernet, EthernetUDP* ethudp) {
    //my_delegate_t delegate;

    _ethernet = ethernet;
    _ethudp = ethudp;

    //broadcasting = true;
    _timestamp = millis() + 2000;
    _mid = random(65536 / 2);

    /*register built-in resource discovery resource */
    _resource_counter = 0;
    
    _observe_counter = 1;
    _observer_slot_counter = 0;
#ifdef ENABLE_OBSERVE
    for (int i = 0; i < CONF_MAX_OBSERVERS; i++) {
        _observer[i].resource = NULL;
    }
#endif
    
    //_resource[_resource_counter] = resource_t(".well-known/core", GET, delegate, true, 0, APPLICATION_LINK_FORMAT);
    //_resource_counter++;

    _retransmit_slot_counter = 0;
    _retransmit = (retransmit_t**) malloc(CONF_MAX_RETRANSMIT_SLOTS * sizeof (retransmit_t*));

//    _observer = (observer_t**) malloc(CONF_MAX_OBSERVERS * sizeof (observer_t*));

    //_packet_buffer = (uint8_t*) malloc(UDP_TX_PACKET_MAX_SIZE * sizeof (uint8_t));
    //_helper_buffer = (uint8_t*) malloc(CONF_HELPER_BUF_LEN * sizeof (uint8_t));
    //_send_buffer = (uint8_t*) malloc(CONF_MAX_MSG_LEN * sizeof (uint8_t));
    //_large_buffer = (uint8_t*) malloc(CONF_LARGE_BUF_LEN * sizeof (uint8_t));
}

void Coap::handler() {
    if (_timestamp <= millis() - 60) {
        // for testing
        digitalWrite(9, HIGH);
        // broadcast every 1000ms
        _timestamp = millis() + 1000;
        delay(50);
        //call every sensor's check function to update their data
        coap_check();
        
        digitalWrite(9, LOW);
        // notify observers
        coap_notify();
	//coap_notify_from_timer();
        // retransmit if needed
        coap_retransmit_loop();
        

    }
    int packet_len = _ethudp->parsePacket();
    if (packet_len) {
        Serial.print("Receiving from ");
                Serial.print(_ethudp->remoteIP());
                Serial.print(":");
                Serial.println(_ethudp->remotePort());                
                _ethudp->read(_packet_buffer, packet_len);
        //call the receiver
        receiver(_packet_buffer, _ethudp->remoteIP(), _ethudp->remotePort(), packet_len);
    } 
}

//void Coap::add_resource(String name, uint8_t methods, my_delegate_t callback,
//        bool fast_resource, uint16_t notify_time, uint8_t content_type) {
//    if (_resource_counter < CONF_MAX_RESOURCES) {
//        _resource[_resource_counter] = resource_t(name, methods, callback, fast_resource, notify_time, content_type);
//        _resource_counter++;
//    }
//}
void Coap::add_resource(CoapSensor * sensor) {
    // remove if this resource is already stored (if we need to update)
    //remove_resource( name );
    // create new resource object
    size_t output_data_len;
    sensor->get_value(_helper_buffer, &output_data_len);
    sensor->set_value(_helper_buffer, 1, _helper_buffer, &output_data_len);
    _resource[_resource_counter++] = resource_t(sensor);
    // push it to the vector
    //resources_.push_back( new_resource );
}

void Coap::update_resource(String name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type) {
    // TODO
    //find and update
}

void Coap::remove_resource(String name) {
    /*
    for(int i = 0; i < _resource_counter; i++) {
            if(_resource[i]->name() == name) {
                    freeResourceSlot(_resource[i]);
                    return;
            }
    }
     */
}

resource_t Coap::resource(uint8_t resource_id) {
    // return the resource object
    return _resource[resource_id];
}

/**
 * Generates the body of the response to a new .well-known/core request message.
 */
coap_status_t Coap::resource_discovery(uint8_t method, uint8_t* input_data, size_t input_data_len,
        uint8_t* output_data, size_t* output_data_len, queries_t queries) {
    // resource discovery function (respond to .well-known/core)
    if (method == COAP_GET) {

        char* output = (char*) output_data;
        //String output;
        size_t i, index = 0;
        for (i = 0; i < _resource_counter; i++) {
            strcpy(output + index, "<");
            index++;
            _resource[i].nameToStr(output + index, _resource[i].name_length() + 1);
            index += _resource[i].name_length();

            strcpy(output + index, ">,");
            index += 2;

            //output.concat("<");
            //output.concat(resources_[i].name());
            //output.concat(">,");
        }
        //int strlen = output.length();
        //int strlen = resources_str.length() ;
        // print it to char array
        //output.toCharArray( (char*)output_data, strlen);

        // delete the last char ","
        output_data[index - 1] = '\0';
        // set output data len
        *output_data_len = index;
        // return status
        return CONTENT;
    }
    /*
       uint8_t index=0;
       uint8_t rid;
       //String output;// = String("<.well-known/core>;ct=40");
       for( rid = 0; rid < CONF_MAX_RESOURCES; rid++ )
       {
          if( resources_[rid].is_set() == true )
          {
             // ARDUINO
             //output += "<" + resources_[rid].name() + ">;ct=" + resources_[rid].content_type() + ",";
             index += sprintf( (char*)output_data + index, "<%s>,", resources_[rid].name() );
          }
       }
       output_data[index-1] = '\0';
     *output_data_len = strlen( (char*)output_data );
       //DBG(mySerial_->println(data));
       //return largeBuf_;
       return CONTENT;
     * */
}

/**
 * Handles new incoming messages from ethernet shield.
 */
void Coap::receiver(uint8_t* buf, IPAddress from, uint16_t port, uint8_t len) {
    // Used to identify if this packet is a CoAP packet (not a CoAP feature)
    //if(buf[0] != WISELIB_MID_COAP) {
    //	return;
    //}

    Serial.println("Coap::receiver");

    coap_status_t coap_error_code;
    coap_packet_t msg;
    coap_packet_t response;
    uint8_t resource_id = 0;
    size_t output_data_len = 0;

    msg.init();
    response.init();

    //memset(_large_buffer, 0, CONF_LARGE_BUF_LEN);
    //Serial.println("memset-called");

    // parse the message
    coap_error_code = msg.buffer_to_packet(len, buf, (char*) _helper_buffer);
    if (msg.version_w() != COAP_VERSION) {
        Serial.println("BAD_REQUEST");
        coap_error_code = BAD_REQUEST;
    }
    if (coap_error_code == COAP_NO_ERROR) {

        // If URI_HOST is set and the HOST doesn't match this host, reject the message
        //FIXME: check what uri_host looks like
        //if((msg.is_option(URI_HOST)) && (msg.uri_host_w() != _ethernet->localIP())) {
        //	return;
        //}
        //empty msg, ack, or rst

        if (msg.code_w() == 0) {
#ifdef ENABLE_OBSERVE
            if (msg.type_w() == RST) {
                coap_remove_observer(msg.mid_w());
            }
#endif
            coap_unregister_con_msg(msg.mid_w(), 1); //FIXME: was 0
            return; // nothing else to do
        }
        // message is a request
        if (msg.code_w() <= 4) { // 1-4
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
            Serial.println("Checking well");

             if (make_string(msg.uri_path_w(), msg.uri_path_len_w()) == String(".well-known/core")) {
                Serial.println("is well");
                if (msg.isGET()) {
                    response.set_code(resource_discovery(msg.code_w(), msg.payload_w(), msg.payload_len_w(), _large_buffer, &output_data_len, msg.uri_queries_w()));
                    // set the content type
                    response.set_option(CONTENT_TYPE);
                    response.set_content_type(APPLICATION_LINK_FORMAT);
                    // check for blockwise response
                    int offset = coap_blockwise_response(&msg, &response, (uint8_t**) & _large_buffer, &output_data_len);
                    // set the payload and length
                    response.set_payload(_large_buffer + offset);
                    response.set_payload_len(output_data_len);
                    //digitalWrite(2,HIGH);
                }// end of method is allowed
                else {
                    //DBG(mySerial_->println("REC::METHOD_NOT_ALLOWED"));
                    response.set_code(METHOD_NOT_ALLOWED);
                }
            }                // find the requested resource
            else if ((res = find_resource(make_string(msg.uri_path_w(), msg.uri_path_len_w()))) != NULL) {
                //DBG(mySerial_->println("REC::RESOURCE FOUND"));
                // check if the requested method is allowed on this resource
                if (res->method_allowed(msg.code_w())) {
                    //DBG(mySerial_->println("REC::METHOD_ALLOWED"));
                    // in case of slow reply send the ACK if this is needed
                    if (res->fast_resource() == false && response.type_w() == ACK) {
                        // send the ACK
                        coap_send(&response, from, port);
                        // init the response again
                        response.init();
                        response.set_type(CON);
                        response.set_mid(coap_new_mid());
                    }
                    // execute the resource and set the status to the response object
                    response.set_code(res->execute(msg.code_w(), msg.payload_w(),
                            msg.payload_len_w(), _large_buffer,
                            &output_data_len, msg.uri_queries_w()));
                    // set the content type
                    response.set_option(CONTENT_TYPE);
                    response.set_content_type(res->content_type());
                    // check for blockwise response
                    int offset = coap_blockwise_response(&msg, &response, (uint8_t**) & _large_buffer, &output_data_len);
                    // set the payload and length
                    response.set_payload(_large_buffer + offset);
                    response.set_payload_len(output_data_len);

#ifdef ENABLE_OBSERVE
                    // if it is set, register the observer
                    if (msg.code_w() == COAP_GET && msg.is_option(OBSERVE)
                            && res->notify_time_w() > 0 && msg.is_option(TOKEN)) {
                        if (coap_add_observer(&msg, &from, port, res) == 1) {
                            response.set_option(OBSERVE);
                            response.set_observe(_observe_counter);
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
            coap_send(&response, from, port);
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
                    coap_send(&response, from, port);
                    //DBG(mySerial_->println("ACTION: Sent ACK"));
                    break;
                case ACK:
                    coap_unregister_con_msg(msg.mid_w(), 1); //FIXME: was 0
                    break;
                case RST:
#ifdef ENABLE_OBSERVE
                    coap_remove_observer(msg.mid_w());
#endif
                    coap_unregister_con_msg(msg.mid_w(), 1); //FIXME: was 0
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
            coap_send(&response, from, port);
            //DBG(mySerial_->println("ACTION: Sent reply"));
        }
    } // end of coap receiver

    void Coap::udp_send(IPAddress ip, uint16_t port, const uint8_t* buffer, size_t size) {
        _ethudp->beginPacket(ip, port);
        _ethudp->write(buffer, size);
        _ethudp->endPacket();
    }

    void Coap::coap_send(coap_packet_t* msg, IPAddress dest, uint16_t port) {
        memset(_send_buffer, 0, CONF_MAX_MSG_LEN);
        uint8_t data_len = msg->packet_to_buffer(_send_buffer);
        if ((msg->type_w() == CON && _retransmit_slot_counter < CONF_MAX_RETRANSMIT_SLOTS)) {
            coap_register_con_msg(dest, port, msg->mid_w(), _send_buffer, data_len, 0);
        } else {
            msg->set_type(NON);
        }
        DBG(Serial.print("Sending to ");
                Serial.print(dest);
                Serial.print(":");
                Serial.println(port);
                )
                udp_send(dest, port, _send_buffer, data_len);
        //DBG(debug_msg(_sendBuffer, data_len));
    }

    uint16_t Coap::coap_new_mid() {
        return _mid++;
    }

    CoapResource* Coap::find_resource(String uri_path) {
    Serial.println("Coap::find_resource");

    for (int i = 0; i < _resource_counter; i++) {
	//DBG(mySerial_->println(resources_[*i].name()));
        Serial.println(_resource[i].name());        
        
	if (uri_path == _resource[i].name()) {
	    return &(_resource[i]);
	}
    }
    Serial.println("Not found");
    return NULL;
}

    int Coap::coap_blockwise_response(coap_packet_t* req, coap_packet_t* resp, uint8_t** data, size_t * data_len) {
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
        return (0);
    }

    uint8_t Coap::coap_register_con_msg(IPAddress ip, uint16_t port, uint16_t mid, uint8_t* buf,
            uint8_t size, uint8_t tries) {
        if (_retransmit_slot_counter < CONF_MAX_RETRANSMIT_SLOTS) {
            uint8_t i = _retransmit_slot_counter;
            _retransmit_slot_counter++;
            _retransmit[i] = allocateRetransmitSlot();
            _retransmit[i]->reg = 1;
            _retransmit[i]->ip = ip;
            _retransmit[i]->port = port;
            _retransmit[i]->mid = mid;
            _retransmit[i]->timeout_and_tries = (CONF_COAP_RESPONSE_TIMEOUT << 4) | tries;
            _retransmit[i]->size = size;
            memcpy(_retransmit[i]->packet, buf, size);
            // ARDUINO
            _timeout = 1000 * (_retransmit[i]->timeout_and_tries >> 4);
            _retransmit[i]->timestamp = millis() + _timeout;
            DBG(Serial.print("Allocated new RETRANSMIT slot ");
                    Serial.print(i);
                    Serial.print(" for message ");
                    Serial.println(_retransmit[i]->mid);
                    //Serial.println("Registered con msg ");
                    )
            return (1);
        } else {
            DBG(Serial.println("Failed to register con msg ");)
            return (0);
        }
    }

    uint8_t Coap::coap_unregister_con_msg(uint16_t mid, uint8_t flag) {
        uint8_t i = 0;

        while (i < _retransmit_slot_counter) {
            if (_retransmit[i]->mid == mid) {
                DBG(Serial.print("Freed RETRANSMIT slot ");
                        Serial.print(i);
                        Serial.print(" for message ");
                        Serial.println(_retransmit[i]->mid);
                        //Serial.println("Unregistered con msg");
                        )
                        uint8_t ret_value = 0x0F & _retransmit[i]->timeout_and_tries;
                freeRetransmitSlot(_retransmit[i], i);
                return ret_value;
            }
            i++;
        }
        return 0;
    }

    void Coap::coap_retransmit_loop(void) {
        //DBG(Serial.println("_retransmit loop");)
        uint8_t i;
        uint8_t _timeoutfactor = 0x01;
        for (i = 0; i < _retransmit_slot_counter; i++) {
            //DBG(mySerial_->println(_retransmit_register_[i]));
            if (_retransmit[i]->reg == 1) {
                // -60 is used because there is always a fault in time
                if (_retransmit[i]->timestamp <= millis() - 60) {
                    _retransmit[i]->timeout_and_tries += 1;
                    _timeoutfactor = _timeoutfactor << (0x0F & _retransmit[i]->timeout_and_tries);
                    // ARDUINO
                    //DBG(mySerial_->println("_retransmit"));
                    udp_send(_retransmit[i]->ip, _retransmit[i]->port,
                            _retransmit[i]->packet, _retransmit[i]->size);

                    if ((0x0F & _retransmit[i]->timeout_and_tries) == CONF_COAP_MAX_RETRANSMIT_TRIES) {
#ifdef ENABLE_OBSERVE
                        coap_remove_observer(_retransmit[i]->mid);
#endif
                        coap_unregister_con_msg(_retransmit[i]->mid, 1);
                        return;
                    } else {
                        // ARDUINO
                        _timeout = _timeoutfactor * 1000 * (_retransmit[i]->timeout_and_tries >> 4);
                        _retransmit[i]->timestamp = millis() + _timeout;
                        return;
                    }
                }
            }
        }
    }


#ifdef ENABLE_OBSERVE
    //FIXME: It is possible that the we need port to identify each client so we can have many observers

    uint8_t Coap::coap_add_observer(coap_packet_t* msg, IPAddress* ip, uint16_t port, CoapResource* resource) {
		    for (uint8_t i = 0; i < CONF_MAX_OBSERVERS; i++) {

	if ((_observer[i].resource == resource)) {
	    //update token
	    memset(_observer[i].token, 0, _observer[i].token_len);
	    _observer[i].token_len = msg->token_len_w();
	    memcpy(_observer[i].token, msg->token_w(), msg->token_len_w());
	    return 1;
	}
	if (_observer[i].resource == NULL) {
		_observer[i].ip = *ip;
	    _observer[i].token_len = msg->token_len_w();
	    memcpy(_observer[i].token, msg->token_w(), msg->token_len_w());
	    _observer[i].resource = resource;
	    _observer[i].last_mid = msg->mid_w();
	    // ARDUINO
	    //		  observers[i].observe_timestamp_ = millis() + 1000*resources_[resource].notify_time_w();
	    _observer[i].timestamp = millis() + 1000;
	    _observer_slot_counter++;
	    
		/*
        uint8_t i;
        for (i = 0; i < _observer_slot_counter; i++) {
            if ((_observer[i].ip == *ip)
                    && _observer[i].port == port && (_observer[i].resource == resource)) {
                //update token
                memset(_observer[i].token, 0, _observer[i].token_len);
                _observer[i].token_len = msg->token_len_w();
                memcpy(_observer[i].token, msg->token_w(), msg->token_len_w());
                return (1);
            }
        }

        if (i < CONF_MAX_OBSERVERS) {
            _observer_slot_counter++;
            //_observer[i] = allocateObserverSlot();
            _observer[i].ip = *ip;
            _observer[i].port = port;
            _observer[i].token_len = msg->token_len_w();
            memcpy(_observer[i].token, msg->token_w(), msg->token_len_w());
            _observer[i].resource = resource;
            _observer[i].last_mid = msg->mid_w();
            // ARDUINO
            _observer[i].timestamp = millis() + 30000;
            DBG(Serial.print("Allocated new OBSERVER slot ");
                    Serial.print(i);
                    Serial.print(" for resource ");
                    Serial.println(_observer[i].resource);
                    )
            return (1);
        }*/
         return (1);
	}
		}
		return (0);
    }

    void Coap::coap_remove_observer(uint16_t mid) {
		Serial.println("Removing Observer");
        /*uint8_t i;
        for (i = 0; i < _observer_slot_counter; i++) {
            if (_observer[i].last_mid == mid) {
                DBG(Serial.print("Freed OBSERVER slot ");
                        Serial.print(i);
                        Serial.print(" for resource ");
                        Serial.println(_observer[i].resource);
                        )
                        //freeObserverSlot(_observer[i], i);
                return;
            }
        }*/
    }

    void Coap::coap_notify_from_timer() {
        /*
           uint8_t rid;
           for( rid = 0; rid < CONF_MAX_RESOURCES; rid++ )
           {
              if ( ( observers[rid].observe_id_ != 0 ) && ( observers[rid].observe_timestamp_ < millis() -60 ) )
              {
                          /*
                 if ( _resources[rid].interrupt_flag_w() == true )
                 {
                    _resources[rid].set_interrupt_flag( false );
                    //return;
                 }
                 else
                 {
                    coap_notify( rid );
                 }
                        coap_notify( rid );
              }
           }
         */
        coap_notify();
    }

    void Coap::coap_notify_from_interrupt(uint8_t resource_id) {
        //_resources[resource_id].set_interrupt_flag( true );
        //coap_notify( resource_id );
    }

    void Coap::coap_notify() {
        coap_packet_t notification;
        uint8_t notification_size;
        size_t output_data_len;
        uint8_t i;

        //memset(_send_buffer, 0, CONF_MAX_MSG_LEN);
        for (i = 0; i < CONF_MAX_OBSERVERS; i++) {
            CoapResource  * resource = _observer[i].resource;
            if (resource == NULL) continue;
            //if( observers[i].observe_resource_ == resource_id )
            if (_observer[i].timestamp < millis()) {
				Serial.println((char)30+i);
				memset(_send_buffer, 0, CONF_MAX_MSG_LEN);

                _observer[i].timestamp = millis() + 30000;
                // send msg
                notification.init();
                if (_retransmit_slot_counter < CONF_MAX_RETRANSMIT_SLOTS) notification.set_type(CON);
                else notification.set_type(NON);
                notification.set_mid(coap_new_mid());

                notification.set_code(resource->execute(COAP_GET, NULL, 0, _large_buffer,
                        &output_data_len, notification.uri_queries_w()));
                notification.set_option(CONTENT_TYPE);
                notification.set_content_type(resource->content_type());
                notification.set_option(TOKEN);
                notification.set_token_len(_observer[i].token_len);
                notification.set_token(_observer[i].token);
                notification.set_option(OBSERVE);
                notification.set_observe(_observe_counter);

                notification.set_payload(_large_buffer);
                notification.set_payload_len(output_data_len);
                notification_size = notification.packet_to_buffer(_send_buffer);

/* TODO: reenable
                if (notification.type_w() == CON) {
                    coap_register_con_msg(_observer[i].ip, _observer[i].port, notification.mid_w(),
                            _send_buffer, notification_size,
                            coap_unregister_con_msg(_observer[i].last_mid, 1)); //FIXME: was 0
                }
*/

                _observer[i].last_mid = notification.mid_w();

                // ARDUINO
                udp_send(_observer[i].ip, _observer[i].port, _send_buffer, notification_size);
		delay(20);
                _observe_counter++;
		break;
            }
        }
	//_observe_counter++;
        //next notification will have greater observe option
    }
    /*
    uint16_t Coap::observe_counter()
    {
       return observe_counter_;
    }

    void Coap::increase_observe_counter()
    {
       observe_counter_++;
    }
     */
#endif

    String Coap::make_string(char* charArray, size_t charLen) {
        memset(_helper_buffer, 0, CONF_HELPER_BUF_LEN);
        memcpy(_helper_buffer, charArray, charLen);
        _helper_buffer[charLen] = '\0';
        return String((char*) _helper_buffer);
    }

    void Coap::debug_msg(uint8_t* msg, uint8_t len) {
        uint8_t i;
        for (i = 0; i < len; i++) {
            //DBG(mySerial_->print(msg[i], HEX));
        }
        //DBG(mySerial_->println(" end"));
    }

    /**
     * Allocators and cleaners for observers and retrasmit slots.
     */
    resource_t * Coap::allocateResourceSlot() {
        resource_t* new_slot;
        new_slot = (resource_t*) malloc(sizeof (resource_t));
        return (new_slot);
    }

    void Coap::freeResourceSlot(resource_t * slot) {
        free(slot);
    }

    /*observer_t * Coap::allocateObserverSlot() {
        observer_t* new_slot;
        new_slot = (observer_t*) malloc(sizeof (observer_t));
        new_slot->token = (uint8_t*) malloc(8 * sizeof (uint8_t));
        return (new_slot);
    }*/
	/*
    void Coap::freeObserverSlot(observer_t* slot, uint8_t indx) {
        free(slot->token);
        free(slot);
        _observer_slot_counter--;
        if (indx != _observer_slot_counter)
            _observer[indx] = _observer[_observer_slot_counter];
    }*/

    retransmit_t * Coap::allocateRetransmitSlot() {
        retransmit_t* new_slot;
        new_slot = (retransmit_t*) malloc(sizeof (retransmit_t));
        new_slot->packet = (uint8_t*) malloc(CONF_MAX_MSG_LEN * sizeof (uint8_t));
        return (new_slot);
    }

    void Coap::freeRetransmitSlot(retransmit_t* slot, uint8_t indx) {
        free(slot->packet);
        free(slot);
        _retransmit_slot_counter--;
        if (indx != _retransmit_slot_counter)
            _retransmit[indx] = _retransmit[_retransmit_slot_counter];
    }

    void Coap::coap_check(void) {
    Serial.println("coap_check");
    int i;
        for (i = 0; i < _resource_counter; i++) {
        	_resource[i].check();
        }
    }
