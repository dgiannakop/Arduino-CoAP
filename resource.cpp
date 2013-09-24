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

#include "resource.h"

CoapResource::CoapResource() {
}

CoapResource::CoapResource(CoapSensor * sensor) {
    del_ = sensor;
    is_set_ = true;
    observe_token_len_=0;
}

coap_status_t CoapResource::execute(uint8_t method, uint8_t* input_data, size_t input_data_len, uint8_t* output_data, size_t* output_data_len, queries_t queries) {
    if (del_) {

	if (method == COAP_GET)
	    method = GET;
	else if (method == COAP_POST)
	    method = POST;
	return del_->callback(method, input_data, input_data_len, output_data, output_data_len, queries);
    }
}

void CoapResource::check() {
    del_->check();
}

void CoapResource::set_notify_time(uint16_t notify_time) {
    //    notify_time_ = notify_time;
    del_->set_notify_time(notify_time);
}

bool CoapResource::is_set() {
    return is_set_;
}

char * CoapResource::name() {
    return del_->get_name();
}

uint8_t CoapResource::name_length() {
    return strlen(del_->get_name());
}


uint8_t CoapResource::method_allowed(uint8_t method) {
    if (method == 3)
	method = 4;
    else if (method == 4)
	method = 8;
    return del_->get_method() & method;
}

uint16_t CoapResource::notify_time_w() {
    return del_->get_notify_time();
}

bool CoapResource::fast_resource() {
    return del_->get_fast();
}

uint8_t CoapResource::content_type() {
    return del_->get_content_type();
}

bool CoapResource::is_changed() {
    return del_->is_changed();
}

void CoapResource::mark_notified() {
    return del_->mark_notified();
}
