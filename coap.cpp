#include "coap.h"

#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif

#ifdef DEBUG
void Coap::init(SoftwareSerial *mySerial, EthernetCoap *ethcoap, uint8_t *buf, char *largeBuf)
{
	my_delegate_t delegate;
	mySerial_ = mySerial;

	broadcasting = true;
	timestamp = millis() + 2000;
	mid_ = random(65536 / 2);
	observe_counter_ = 1;
	//register built-in resource discovery resource
	delegate = fastdelegate::MakeDelegate(this, &Coap::resource_discovery);
	resource_t discovery(".well-known/core", GET, delegate, true, 0, APPLICATION_LINK_FORMAT);
	resources_.push_back(discovery);

	ethcoap_ = ethcoap;
}
#else
void Coap::init(EthernetClass* ethernet, EthernetCoap* ethcoap)
{
	my_delegate_t delegate;

	broadcasting = true;
	timestamp = millis() + 2000;
	mid_ = random(65536 / 2);
	observe_counter_ = 1;
	//register built-in resource discovery resource
	delegate = fastdelegate::MakeDelegate(this, &Coap::resource_discovery);
	resource_t discovery(".well-known/core", GET, delegate, true, 0, APPLICATION_LINK_FORMAT);
	resources_.push_back(discovery);

	_ethernet = ethernet;
	_ethcoap = ethcoap;
}
#endif


void Coap::handler()
{
	if(timestamp <= millis() - 60) {
		// for testing
		digitalWrite(12, HIGH);
		// broadcast every 1000ms
		timestamp = millis() + 1000;
		// if broadcasting is set, send the broadcast message (not a CoAP feature)
		if(broadcasting == true) {
			delay(200);
			helperBuf_[0] = 0x01;
			//tx_ = Tx16Request(0xffff, helperBuf_, 1);
			//xbee_->send(tx_, 112);
			_ethcoap->beginPacket(_ethcoap->getBroadCast(), 5683);
			_ethcoap->write(helperBuf_, CONF_HELPER_BUF_LEN);
			_ethcoap->endPacket();
		} else {
			delay(50);
		}
		digitalWrite(12, LOW);
		// notify observers
		coap_notify_from_timer();
		// retransmit if needed
		coap_retransmit_loop();
	}
	//if(xbee_->checkForData(112)) {
	int packet_len = _ethcoap->parsePacket();
	if(packet_len) {
		//get our response and save it on our response variable
		//xbee_->getResponse().getRx16Response(*rx_);
		_ethcoap->read(_ethcoap->packetBuffer, packet_len);
		//call the receiver
		receiver(_ethcoap->packetBuffer, _ethcoap->remoteIP(), packet_len);
		//receiver(xbee_->getResponse().getData(), rx_->getRemoteAddress16(), xbee_->getResponse().getDataLength());
	}

}


void Coap::receiver(uint8_t *buf, IPAddress from, uint8_t len)
{
	// Used to identify if this packet is a CoAP packet (not a CoAP feature)
	//if(buf[0] != WISELIB_MID_COAP) {
	//	return;
	//}
	coap_status_t coap_error_code;
	coap_packet_t msg;
	coap_packet_t response;
	uint8_t resource_id = 0;
	uint8_t output_data[CONF_LARGE_BUF_LEN];
	size_t output_data_len = 0;

	msg.init();
	response.init();

	memset(output_data, 0, CONF_LARGE_BUF_LEN);
	// parse the message
	coap_error_code = msg.buffer_to_packet(len, buf, (char *)helperBuf_);
	if(msg.version_w() != COAP_VERSION) {
		coap_error_code = BAD_REQUEST;
	}
	if(coap_error_code == COAP_NO_ERROR) {
		// if URI_HOST is set and the HOST doesn't much this host, reject the message
		//FIXME: check what uri_host looks like
		//if((msg.is_option(URI_HOST)) && (msg.uri_host_w() != _ethernet->localIP())) {
		//	return;
		//}
		//empty msg, ack, or rst
		if(msg.code_w() == 0) {
			coap_unregister_con_msg(msg.mid_w(), 0);
			#ifdef OBSERVING
			if(msg.type_w() == RST) {
				coap_remove_observer(msg.mid_w());
			}
			#endif
			return; // nothing else to do
		}
		// message is a request
		if(msg.code_w() <= 4) {  // 1-4
			switch(msg.type_w()) {
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

			// find the requested resource
			if(find_resource(&resource_id, make_string(msg.uri_path_w(), msg.uri_path_len_w())) == true) {
				//DBG(mySerial_->println("REC::RESOURCE FOUND"));
				// check if the requested method is allowed on this resource
				if(resources_[resource_id].method_allowed(msg.code_w())) {
					//DBG(mySerial_->println("REC::METHOD_ALLOWED"));
					// in case of slow reply send the ACK if this is needed
					if(resources_[resource_id].fast_resource() == false && response.type_w() == ACK) {
						// send the ACK
						coap_send(&response, from);
						// init the response again
						response.init();
						response.set_type(CON);
						response.set_mid(coap_new_mid());
					}
					// execute the resource and set the status to the response object
					response.set_code(resources_[resource_id].execute(msg.code_w(), msg.payload_w(), msg.payload_len_w(), output_data, &output_data_len, msg.uri_queries_w()));
					// set the content type
					response.set_option(CONTENT_TYPE);
					response.set_content_type(resources_[resource_id].content_type());
					// check for blockwise response
					coap_blockwise_response(&msg, &response, (uint8_t **)&output_data, &output_data_len);
					// set the payload and length
					response.set_payload(output_data);
					response.set_payload_len(output_data_len);

					#ifdef OBSERVING
					// if it is set, register the observer
					if(msg.code_w() == COAP_GET && msg.is_option(OBSERVE) && resources_[resource_id].notify_time_w() > 0 && msg.is_option(TOKEN)) {
						if(coap_add_observer(&msg, &from, resource_id) == 1) {
							response.set_option(OBSERVE);
							response.set_observe(observe_counter());
						}
					} // end of add observer
					#endif
				} // end of method is allowed
				else {
					//DBG(mySerial_->println("REC::METHOD_NOT_ALLOWED"));
					response.set_code(METHOD_NOT_ALLOWED);
				} // if( method_allowed )
			} // end of resource found
			else {
				//DBG(mySerial_->println("REC::NOT_FOUND"));
				response.set_code(NOT_FOUND);
			}
			// if the request has a token, add it to the response
			if(msg.is_option(TOKEN)) {
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
		if(msg.code_w() >= 64 && msg.code_w() <= 191) {
			//DBG(mySerial_->println("REC::RESPONSE"));
			switch(msg.type_w()) {
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
					#ifdef OBSERVING
					coap_remove_observer(msg.mid_w());
					#endif
					coap_unregister_con_msg(msg.mid_w(), 0);
					break;
			}
			return;
		}
	} // end of no error found
	else {
		// error found
		response.set_code(coap_error_code);
		if(msg.type_w() == CON) {
			response.set_type(ACK);
			response.set_mid(msg.mid_w());
		} else
			response.set_type(NON);
		coap_send(&response, from);
		//DBG(mySerial_->println("ACTION: Sent reply"));
	}
} // end of coap receiver


void Coap::add_resource(String name, uint8_t methods, my_delegate_t callback, bool fast_resource, uint16_t notify_time, uint8_t content_type)
{
	// remove if this resource is already stored (if we need to update)
	//remove_resource( name );
	// create new resource object
	resource_t new_resource(name, methods, callback, fast_resource, notify_time, content_type);
	// push it to the vector
	resources_.push_back(new_resource);
}


void Coap::update_resource(String name, uint8_t methods, bool fast_resource, int notify_time, uint8_t content_type)
{
	// TODO
	//find and update
}


void Coap::remove_resource(String name)
{
	for(int i = 0; i < resources_.size(); i++) {
		if(resources_[i].name() == name) {
			resources_.remove(i);
		}
	}
}


resource_t Coap::resource(uint8_t resource_id)
{
	// return the resource object
	return resources_[resource_id];
}


coap_status_t Coap::resource_discovery(uint8_t method, uint8_t *input_data, size_t input_data_len, uint8_t *output_data, size_t *output_data_len, queries_t queries)
{
	// resource discovery function (respond to .well-known/core
	if(method == COAP_GET) {
		// the first time the device gets a request for this, disables broadcasting (not a CoAP feature)
		if(broadcasting == true) {
			broadcasting = false;
		}
		// build the response string
		uint8_t rid;
		String output;// = String("<.well-known/core>;ct=40");
		for(rid = 0; rid < resources_.size(); rid++) {
			// ARDUINO
			output += "<" + resources_[rid].name() + ">;ct=" + resources_[rid].content_type() + ",";
		}
		// print it to char array
		output.toCharArray((char *)output_data, CONF_LARGE_BUF_LEN);
		// delete the last char ","
		output_data[output.length()-1] = '\0';
		// set output data len
		*output_data_len = strlen((char *)output_data);
		// return status
		return CONTENT;
	}
}


void Coap::coap_send(coap_packet_t *msg, IPAddress dest)
{
	memset(sendBuf_, 0, CONF_MAX_MSG_LEN);
	uint8_t data_len = msg->packet_to_buffer(sendBuf_);
	if((msg->type_w() == CON)) {
		coap_register_con_msg(dest, msg->mid_w(), sendBuf_, data_len, 0);
	}
	_ethcoap->beginPacket(dest, _ethcoap->remotePort());
	_ethcoap->write(sendBuf_, data_len);
	_ethcoap->endPacket();
	//tx_ = Tx16Request(dest, sendBuf_, data_len);
	//xbee_->send(tx_, 112);
	DBG(debug_msg(sendBuf_, data_len));
}


uint16_t Coap::coap_new_mid()
{
	return mid_++;
}


bool Coap::find_resource(uint8_t *i, String uri_path)
{
	for((*i) = 0; (*i) < resources_.size(); (*i)++) {
		//DBG(mySerial_->println(resources_[*i].name()));
		if(uri_path == resources_[*i].name()) {
			return true;
		}
	}
	return false;
}


void Coap::coap_blockwise_response(coap_packet_t *req, coap_packet_t *resp, uint8_t **data, size_t *data_len)
{
	if(req->is_option(BLOCK2)) {
		if(req->block2_size_w() > CONF_MAX_PAYLOAD_LEN) {
			resp->set_block2_size(CONF_MAX_PAYLOAD_LEN);
			resp->set_block2_num(req->block2_num_w()*req->block2_size_w() / CONF_MAX_PAYLOAD_LEN);
		} else {
			resp->set_block2_size(req->block2_size_w());
			resp->set_block2_num(req->block2_num_w());
		}
		if(*data_len < resp->block2_size_w()) {
			resp->set_block2_more(0);
		} else if((*data_len - req->block2_offset_w()) > resp->block2_size_w()) {
			resp->set_block2_more(1);
			*data_len = resp->block2_size_w();
		} else {
			resp->set_block2_more(0);
			*data_len -= req->block2_offset_w();
		}
		resp->set_option(BLOCK2);
		*data = *data + req->block2_offset_w();
		return;
	}
	if(*data_len > CONF_MAX_PAYLOAD_LEN) {
		resp->set_option(BLOCK2);
		resp->set_block2_num(0);
		resp->set_block2_more(1);
		resp->set_block2_size(CONF_MAX_PAYLOAD_LEN);
		*data_len = CONF_MAX_PAYLOAD_LEN;
	}
}


void Coap::coap_register_con_msg(uint16_t id, uint16_t mid, uint8_t *buf, uint8_t size, uint8_t tries)
{
	DBG(mySerial_->println("Registered con msg "));
	uint8_t i = 0;
	while(i < CONF_MAX_RETRANSMIT_SLOTS) {
		if(retransmit_mid_[i] == 0) {
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


uint8_t Coap::coap_unregister_con_msg(uint16_t mid, uint8_t flag)
{
	DBG(mySerial_->println("Unregistered con msg"));
	uint8_t i = 0;
	while(i < CONF_MAX_RETRANSMIT_SLOTS) {
		if(retransmit_mid_[i] == mid) {
			if(flag == 1) {
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


void Coap::coap_retransmit_loop(void)
{
	//DBG(mySerial_->println("Retransmit loop"));
	uint8_t i;
	uint8_t timeout_factor = 0x01;
	for(i = 0; i < CONF_MAX_RETRANSMIT_SLOTS; i++) {
		//DBG(mySerial_->println(retransmit_register_[i]));
		if(retransmit_register_[i] == 1) {
			// -60 is used because there is always a fault in time
			if(retransmit_timestamp_[i] <= millis() - 60) {
				retransmit_timeout_and_tries_[i] += 1;
				timeout_factor = timeout_factor << (0x0F & retransmit_timeout_and_tries_[i]);
				// ARDUINO
				DBG(mySerial_->println("RETRANSMIT"));
				_ethcoap->beginPacket(_ethcoap->remoteIP(), _ethcoap->remotePort());
				_ethcoap->write(retransmit_packet_[i], retransmit_size_[i]);
				_ethcoap->endPacket();
				//tx_ = Tx16Request(retransmit_id_[i], retransmit_packet_[i], retransmit_size_[i]);
				//xbee_->send(tx_, 112);

				if((0x0F & retransmit_timeout_and_tries_[i]) == CONF_COAP_MAX_RETRANSMIT_TRIES) {
#ifdef OBSERVING
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


#ifdef OBSERVING
uint8_t Coap::coap_add_observer(coap_packet_t *msg, IPAddress* ip, uint8_t resource_id)
{
	uint8_t i, free_slot = 0;
	for(i = 0; i < CONF_MAX_OBSERVERS; i++) {
		if((observe_id_[i].observe_ip_ == *ip) && (observe_resource_[i] == resource_id)) {
			//update token
			memset(observe_token_[i], 0, observe_token_len_[i]);
			observe_token_len_[i] = msg->token_len_w();
			memcpy(observe_token_[i], msg->token_w(), msg->token_len_w());
			return 1;
		}
		//FIXME: make a struct to hold the observers and their IPs
		if(observe_id_[i].observe_id_ = 0) {
			free_slot = i + 1;
		}
	}
	if(free_slot != 0) {
		observe_id_[free_slot-1].observe_id_ = free_slot - 1;
		observe_id_[free_slot-1].observe_ip_ = *ip;
		observe_token_len_[free_slot-1] = msg->token_len_w();
		memcpy(observe_token_[free_slot-1], msg->token_w(), msg->token_len_w());
		observe_resource_[free_slot-1] = resource_id;
		observe_last_mid_[free_slot-1] = msg->mid_w();
		// ARDUINO
		observe_timestamp_[free_slot-1] = millis() + 1000 * resources_[resource_id].notify_time_w();
		return 1;
	}
	return 0;
}


void Coap::coap_remove_observer(uint16_t mid)
{
	uint8_t i;
	for(i = 0; i < CONF_MAX_OBSERVERS; i++) {
		if(observe_last_mid_[i] == mid) {
			observe_last_mid_[i] = 0;
			observe_id_[i].observe_id_ = 0;
			//FIXME: make a struct to hold the observers and their IPs
			observe_resource_[i] = 0;
			memset(observe_token_[i], 0, observe_token_len_[i]);
			observe_token_len_[i] = 0;
			observe_timestamp_[i] = 0;
		}
	}
}


void Coap::coap_notify_from_timer()
{
	uint8_t rid;
	for(rid = 0; rid < CONF_MAX_RESOURCES; rid++) {
		if((observe_id_[rid].observe_id_ != 0) && (observe_timestamp_[rid] <= millis() - 60)) {
			if(resources_[rid].interrupt_flag_w() == true) {
				resources_[rid].set_interrupt_flag(false);
				//return;
			} else {
				coap_notify(rid);
			}
		}
	}
}


void Coap::coap_notify_from_interrupt(uint8_t resource_id)
{
	resources_[resource_id].set_interrupt_flag(true);
	coap_notify(resource_id);
}


void Coap::coap_notify(uint8_t resource_id)
{
	coap_packet_t notification;
	uint8_t notification_size;
	uint8_t output_data[CONF_LARGE_BUF_LEN];
	size_t output_data_len;
	uint8_t i;
	memset(sendBuf_, 0, CONF_MAX_MSG_LEN);
	for(i = 0; i < CONF_MAX_OBSERVERS; i++) {
		if(observe_resource_[i] == resource_id) {
			// send msg
			notification.init();
			notification.set_type(CON);
			notification.set_mid(coap_new_mid());

			notification.set_code(resources_[resource_id].execute(COAP_GET, NULL, 0, output_data, &output_data_len, notification.uri_queries_w()));
			notification.set_option(CONTENT_TYPE);
			notification.set_content_type(resources_[resource_id].content_type());
			notification.set_option(TOKEN);
			notification.set_token_len(observe_token_len_[i]);
			notification.set_token(observe_token_[i]);
			notification.set_option(OBSERVE);
			notification.set_observe(observe_counter());

			notification.set_payload(output_data);
			notification.set_payload_len(output_data_len);
			notification_size = notification.packet_to_buffer(sendBuf_);
			coap_register_con_msg(observe_id_[i].observe_id_, notification.mid_w(),
								  sendBuf_, notification_size, coap_unregister_con_msg(observe_last_mid_[i], 0));
			observe_last_mid_[i] = notification.mid_w();
			// ARDUINO
			_ethcoap->beginPacket(observe_id_[resource_id].observe_ip_, 5683);
			_ethcoap->write(sendBuf_, notification_size);
			_ethcoap->endPacket();
			//tx_ = Tx16Request(observe_id_[i], sendBuf_, notification_size);
			//xbee_->send(tx_, 112);
			observe_timestamp_[i] = millis() + 1000 * resources_[resource_id].notify_time_w();
		}
	}
	increase_observe_counter();
	//next notification will have greater observe option
}


uint16_t Coap::observe_counter()
{
	return observe_counter_;
}


void Coap::increase_observe_counter()
{
	observe_counter_++;
}
#endif


String Coap::make_string(char *charArray, size_t charLen)
{
	memset(helperBuf_, 0, CONF_HELPER_BUF_LEN);
	memcpy(helperBuf_, charArray, charLen);
	helperBuf_[charLen] = '\0';
	return String((char *)helperBuf_);
}


void Coap::debug_msg(uint8_t *msg, uint8_t len)
{
	uint8_t i;
	for(i = 0; i < len; i++) {
		DBG(mySerial_->print(msg[i], HEX));
	}
	DBG(mySerial_->println(" end"));
}
