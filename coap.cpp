#include <coap.h>

#ifdef ENABLE_DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif

#ifdef ENABLE_DEBUG

void Coap::init(SoftwareSerial *mySerial, XBeeRadio *xbee, XBeeRadioResponse *response, Rx16Response *rx, uint8_t* buf, char* largeBuf) {

    mySerial_ = mySerial;

    broadcasting = true;
    timestamp = millis() + 2000;
    mid_ = random(65536 / 2);
    observe_counter_ = 1;
    //register built-in resource discovery resource

    //resource_t discovery( ".well-known/core", GET, &resource_discovery, true, 0, APPLICATION_LINK_FORMAT );
    //resources_.push_back( discovery );

    xbee_ = xbee;
    response_ = response;
    rx_ = rx;
}
#else

void Coap::init(XBeeRadio *xbee, XBeeRadioResponse *response, Rx16Response *rx) {
    rcount = 0;
    String hereiamSTR = String("hereiam");
    hereiamSTR.toCharArray(hereiam, 8);
    last_broadcast = millis();


    broadcasting = true;
    timestamp = millis() + 2000;
    mid_ = random(65536 / 2);
    observe_counter_ = 1;
    //register built-in resource discovery resource

    //resources_[rcount++] = resource_t( ".well-known/core", GET, &Coap::resource_discovery, true, 0, APPLICATION_LINK_FORMAT );
#ifdef ENABLE_OBSERVE
    for (int i = 0; i < CONF_MAX_OBSERVERS; i++) {
        observers[i].observe_resource_ = NULL;
    }
#endif

    xbee_ = xbee;
    response_ = response;
    rx_ = rx;
}
#endif

void Coap::handler() {
    if (timestamp <= millis() - 60) {
        digitalWrite(13, HIGH);
        // for testing

        // broadcast every 1000ms
        timestamp = millis() + 1000;
        if (last_broadcast + 30 * 1000 < millis()) {
            last_broadcast = millis();
            tx_ = Tx16Request(0xffff, (uint8_t*) hereiam, 7);
            xbee_->send(tx_, 112);
        }
        delay(50);

        //call every sensor's check function to update their data
        coap_check();

#ifdef ENABLE_OBSERVE
        // notify observers

        coap_notify();
#endif
        // retransmit if needed
        coap_retransmit_loop();
    }
    if (xbee_->checkForData(112)) {
        //get our response and save it on our response variable
        xbee_->getResponse().getRx16Response(*rx_);
        //call the receiver
        receiver(xbee_->getResponse().getData(), rx_->getRemoteAddress16(), xbee_->getResponse().getDataLength());
    }
    digitalWrite(13, LOW);
}

void Coap::add_resource(CoapSensor * sensor) {
    // remove if this resource is already stored (if we need to update)
    //remove_resource( name );
    // create new resource object
    size_t output_data_len;
    sensor->get_value(helperBuf_, &output_data_len);
    sensor->set_value(helperBuf_, 1, helperBuf_, &output_data_len);
    resources_[rcount++] = resource_t(sensor);
    // push it to the vector
    //resources_.push_back( new_resource );
}

void Coap::update_resource(String name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type) {
    // TODO
    //find and update
}

void Coap::remove_resource(String name) {
    /*
    for( int i = 0; i < resources_.size(); i++ )
    {
  if( resources_[i].name() == name )
  {
     resources_.remove( i );         
  }
    }*/
}

resource_t Coap::resource(uint8_t resource_id) {
    // return the resource object
    return resources_[resource_id];
}

/**
 * Generates the body of the response to a new .well-known/core request message.
 */

coap_status_t Coap::resource_discovery(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries) {

    // resource discovery function (respond to .well-known/core
    if (method == COAP_GET) {

        char * output = (char *) output_data;
        //String output;
        size_t i, index = 0;
        ;
        for (i = 0; i < rcount; i++) {

            strcpy(output + index, "<");
            index++;
            resources_[i].nameToStr(output + index, resources_[i].name_length() + 1);
            index += resources_[i].name_length();

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
    if (coap_error_code == NO_ERROR) {

        uint16_t address = xbee_->myAddress;
        uint8_t * bit = ((uint8_t*) & address);
        uint8_t mbyte = bit[1];
        uint8_t lbyte = bit[0];
        bit[0] = mbyte;
        bit[1] = lbyte;

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

            if (make_string(msg.uri_path_w(), msg.uri_path_len_w()) == String(".well-known/core")) {
                if (msg.isGET()) {
                    response.set_code(resource_discovery(msg.code_w(), msg.payload_w(), msg.payload_len_w(), output_data, &output_data_len, msg.uri_queries_w()));
                    // set the content type
                    response.set_option(CONTENT_TYPE);
                    response.set_content_type(APPLICATION_LINK_FORMAT);
                    // check for blockwise response
                    int offset = coap_blockwise_response(&msg, &response, (uint8_t**) & output_data, &output_data_len);
                    // set the payload and length
                    response.set_payload(output_data + offset);
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
                    int offset = coap_blockwise_response(&msg, &response, (uint8_t**) & output_data, &output_data_len);
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
    tx_ = Tx16Request(dest, sendBuf_, data_len);
    xbee_->send(tx_, 112);
    DBG(debug_msg(sendBuf_, data_len));
}

uint16_t Coap::coap_new_mid() {
    return mid_++;
}

CoapResource* Coap::find_resource(String uri_path) {
    for (int i = 0; i < rcount; i++) {
	//DBG(mySerial_->println(resources_[*i].name()));
	if (uri_path == resources_[i].name()) {
	    return &(resources_[i]);
	}
    }
    return NULL;
}

int Coap::coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len) {
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
    DBG(mySerial_->println("Registered con msg "));
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
    DBG(mySerial_->println("Unregistered con msg"));
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
		DBG(mySerial_->println("RETRANSMIT"));
		tx_ = Tx16Request(retransmit_id_[i], retransmit_packet_[i], retransmit_size_[i]);
		xbee_->send(tx_, 112);

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

void Coap::coap_remove_observer(uint16_t mid) {
    uint8_t i;
    for (i = 0; i < CONF_MAX_OBSERVERS; i++) {
	if (observers[i].observe_last_mid_ == mid) {
	    observers[i].observe_last_mid_ = 0;
	    observers[i].observe_id_ = 0;
	    observers[i].observe_resource_ = NULL;
	    memset(observers[i].observe_token_, 0, observers[i].observe_token_len_);
	    observers[i].observe_token_len_ = 0;
	    observers[i].observe_timestamp_ = 0;
	}
    }
}

void Coap::coap_notify() {
    for (int i = 0; i < CONF_MAX_OBSERVERS; i++) {

	if (observers[i].observe_resource_ == NULL) continue;

	observer_t * observer = &(observers[i]);
	CoapResource* resource = observer->observe_resource_;

	if ((observers[i].observe_timestamp_ < millis()) || (resource->is_changed())) {
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
	    tx_ = Tx16Request(observer->observe_id_, sendBuf_, notification_size);
	    xbee_->send(tx_, 112);
	    delay(20);
	}
    }


}

/*uint16_t Coap::observe_counter()
{
   return observe_counter_;
}

void Coap::increase_observe_counter()
{
   observe_counter_++;
}*/
#endif

String Coap::make_string(char* charArray, size_t charLen) {
    memset(helperBuf_, 0, CONF_HELPER_BUF_LEN);
    memcpy(helperBuf_, charArray, charLen);
    helperBuf_[charLen] = '\0';
    return String((char*) helperBuf_);
}

void Coap::debug_msg(uint8_t* msg, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
	DBG(mySerial_->print(msg[i], HEX));
    }
    DBG(mySerial_->println(" end"));
}

void Coap::coap_check(void) {
    int i;
    for (i = 0; i < rcount; i++) {
	resources_[i].check();
    }
}
