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

#include "packet.h"

void CoapPacket::init(void) {
    version_ = COAP_VERSION;
    type_ = 0;
    opt_count_ = 0;
    code_ = 0;
    mid_ = 0;
    options_ = 0x00;
    uri_path_len_ = 0;
    payload_len_ = 0;
}

coap_status_t CoapPacket::buffer_to_packet(uint8_t len, uint8_t* buf, char* largeBuf) {
    //header
    version_ = (COAP_HEADER_VERSION_MASK & buf[1]) >> COAP_HEADER_VERSION_SHIFT;
    type_ = (COAP_HEADER_TYPE_MASK & buf[1]) >> COAP_HEADER_TYPE_SHIFT;
    opt_count_ = (COAP_HEADER_OPT_COUNT_MASK & buf[1]) >> COAP_HEADER_OPT_COUNT_SHIFT;
    code_ = buf[2];
    mid_ = buf[3] << 8 | buf[4];

    //options
    uint8_t *current_opt = buf + COAP_HEADER_LEN + 1;
    if (opt_count_) {

	uint16_t opt_len = 0;
	uint8_t opt_index = 0;
	uint8_t current_delta = 0;
	for (opt_index = 0; opt_index < opt_count_; opt_index++) {

	    current_delta += current_opt[0] >> 4;
	    //get option length
	    if ((0x0F & current_opt[0]) < 15) {
		opt_len = 0x0F & current_opt[0];
		current_opt += 1; //point to option value
	    } else {
		opt_len = current_opt[1] + 15;
		current_opt += 2; //point to option value
	    }
	    if (current_delta == 14 && opt_len == 0) // fence post
	    {
		continue;
	    }

	    switch (current_delta) {
		case CONTENT_TYPE:
		    set_option(CONTENT_TYPE);
		    content_type_ = get_int_opt_value(current_opt, opt_len, false);
		    break;
		case MAX_AGE:
		    set_option(MAX_AGE);
		    max_age_ = get_int_opt_value(current_opt, opt_len, false);
		    break;
		case PROXY_URI:
		    set_option(PROXY_URI);
		    break;
		case ETAG:
		    set_option(ETAG);
		    break;
		case URI_HOST:
		    // based on id, not ip-literal
		    set_option(URI_HOST);
		    uri_host_ = get_int_opt_value(current_opt, opt_len, true);
		    break;
		case LOCATION_PATH:
		    set_option(LOCATION_PATH);
		    break;
		case URI_PORT:
		    set_option(URI_PORT);
		    uri_port_ = get_int_opt_value(current_opt, opt_len, false);
		    break;
		case LOCATION_QUERY:
		    set_option(LOCATION_QUERY);
		    break;
		case URI_PATH:
		    set_option(URI_PATH);
		    merge_options(&uri_path_, &uri_path_len_, current_opt, opt_len, '/');
		    break;
		case OBSERVE:
		    set_option(OBSERVE);
		    observe_ = get_int_opt_value(current_opt, opt_len, false);
		    break;
		case TOKEN:
		    set_option(TOKEN);
		    token_len_ = opt_len; // may fix
		    memcpy(token_, current_opt, opt_len); // may fix
		    break;
		case ACCEPT:
		    set_option(ACCEPT);
		    accept_ = get_int_opt_value(current_opt, opt_len, false);
		    break;
		case IF_MATCH:
		    set_option(IF_MATCH);
		    break;
		case MAX_OFE:
		    set_option(MAX_OFE);
		    break;
		case URI_QUERY:
		    set_option(URI_QUERY);
		    make_uri_query(current_opt, opt_len, largeBuf);
		    //merge_options( &uri_query_, &uri_query_len_, current_opt, opt_len, '&' );
		    break;
		case BLOCK2:
		    set_option(BLOCK2);
		    block2_num_ = get_int_opt_value(current_opt, opt_len, false);
		    block2_more_ = (block2_num_ & 0x08) >> 3;
		    block2_size_ = 16 << (block2_num_ & 0x07);
		    block2_offset_ = ((block2_num_ & 0xF0) >> 4) * block2_size_;
		    block2_num_ = ((block2_num_ & 0xF0) >> 4)&0x0F;
		    break;
		case BLOCK1:
		    set_option(BLOCK1);
		    break;
		case IF_NONE_MATCH:
		    set_option(IF_NONE_MATCH);
		    break;
		default:
		    return BAD_OPTION;

	    }
	    current_opt += opt_len; // point to next option delta
	}
    }
    //get payload
    payload_ = current_opt;
    payload_len_ = len - (payload_ - buf);

    return COAP_NO_ERROR;
}

uint8_t CoapPacket::packet_to_buffer(uint8_t *buf) {
    //options
    uint8_t current_delta = 0;
    uint8_t buf_index = COAP_HEADER_LEN + 1;
    if (is_option(CONTENT_TYPE)) {
	buf_index += set_int_opt_value(CONTENT_TYPE, current_delta, &buf[buf_index], content_type_);
	current_delta = CONTENT_TYPE;
	opt_count_ += 1;
    }
    if (is_option(MAX_AGE)) {
	buf_index += set_int_opt_value(MAX_AGE, current_delta, &buf[buf_index], max_age_);
	current_delta = MAX_AGE;
	opt_count_++;
    }
    if (is_option(URI_HOST)) {
	buf_index += set_int_opt_value(URI_HOST, current_delta, &buf[buf_index], uri_host_);
	current_delta = URI_HOST;
	opt_count_++;
    }
    if (is_option(URI_PORT)) {
	buf_index += set_int_opt_value(URI_PORT, current_delta, &buf[buf_index], uri_port_);
	current_delta = URI_PORT;
	opt_count_++;
    }
    if (is_option(URI_PATH)) {
	char seperator[] = "/";
	buf_index += split_option(URI_PATH, current_delta, &buf[buf_index], seperator);
	current_delta = URI_PATH;
    }
    if (is_option(OBSERVE)) {
	buf_index += set_int_opt_value(OBSERVE, current_delta, &buf[buf_index], observe_);
	current_delta = OBSERVE;
	opt_count_ += 1;
    }
    if (is_option(TOKEN)) {
	buf[buf_index++] = (TOKEN - current_delta) << 4 | token_len_;
	memcpy(&buf[buf_index], token_, token_len_);
	current_delta = TOKEN;
	buf_index += token_len_;
	opt_count_ += 1;
    }
    if (is_option(ACCEPT)) {
	buf_index += set_int_opt_value(ACCEPT, current_delta, &buf[buf_index], accept_);
	current_delta = ACCEPT;
	opt_count_ += 1;
    }
    if (is_option(BLOCK2)) {
	uint32_t block = block2_num_ << 4;
	if (block2_more_)
	    block |= 0x8;
	block |= 0xF & (power_of_two(block2_size_) - 4);
	buf_index += set_int_opt_value(BLOCK2, current_delta, &buf[buf_index], block);
	current_delta = BLOCK2;
	opt_count_ += 1;
    }
    //header
    buf[0] = WISELIB_MID_COAP;
    buf[1] = COAP_HEADER_VERSION_MASK & version_ << COAP_HEADER_VERSION_SHIFT;
    buf[1] |= COAP_HEADER_TYPE_MASK & type_ << COAP_HEADER_TYPE_SHIFT;
    buf[1] |= COAP_HEADER_OPT_COUNT_MASK & opt_count_ << COAP_HEADER_OPT_COUNT_SHIFT;
    buf[2] = code_;
    buf[3] = 0xFF & (mid_ >> 8);
    buf[4] = 0xFF & mid_;
    //payload
    memcpy(&buf[buf_index], payload_, payload_len_);
    return buf_index + payload_len_;
}

uint8_t CoapPacket::add_fence_opt(uint8_t opt, uint8_t *current_delta, uint8_t *buf) {
    uint8_t i = 0;
    while (opt - *current_delta > 15) {
	uint8_t delta = 14 - (*current_delta % 14);
	set_opt_header(delta, 0, &buf[i++]);
	*current_delta += delta;
	opt_count_++;
    }
    return i;
}

uint8_t CoapPacket::set_opt_header(uint8_t delta, size_t len, uint8_t *buf) {
    if (len < 15) {
	buf[0] = delta << 4 | len;
	return 1;
    } else {
	buf[0] = delta << 4 | 0x0F;
	buf[1] = len - 15;
	return 2;
    }
}

uint8_t CoapPacket::set_int_opt_value(uint8_t opt, uint8_t current_delta, uint8_t *buf, uint32_t value) {
    uint8_t i = add_fence_opt(opt, &current_delta, buf);
    uint8_t start_i = i;

    //uint8_t *option = &buf[i];

    if (0xFF000000 & value) buf[++i] = (uint8_t) (0xFF & value >> 24);
    if (0xFFFF0000 & value) buf[++i] = (uint8_t) (0xFF & value >> 16);
    if (0xFFFFFF00 & value) buf[++i] = (uint8_t) (0xFF & value >> 8);
    if (0xFFFFFFFF & value) buf[++i] = (uint8_t) (0xFF & value);

    i += set_opt_header(opt - current_delta, i - start_i, &buf[start_i]);
    return i;
}

uint16_t CoapPacket::get_int_opt_value(uint8_t *value, uint16_t length, bool hexAscii) {
    uint16_t var = 0;
    int i = 0;
    while (i < length) {
	if (hexAscii == false) {
	    var <<= 8;
	    var |= 0xFF & value[i++];
	} else {
	    var *= 16;
	    if (value[i] >= 0x41 && value[i] <= 0x5a)
		var += value[i] - 0x41 + 10;
	    else if (value[i] >= 0x61 && value[i] <= 0x7a)
		var += value[i] - 0x61 + 10;
	    else if (value[i] >= 0x30 && value[i] <= 0x39)
		var += value[i] - 0x30;
	    i++;
	}
    }
    return var;
}

void CoapPacket::merge_options(char **dst, size_t *dst_len, uint8_t *value, uint16_t length, char seperator) {
    if (*dst_len > 0) {
	(*dst)[*dst_len] = seperator;
	*dst_len += 1;
	memmove((*dst) + (*dst_len), value, length);
	*dst_len += length;
    } else {
	*dst = (char *) value;
	*dst_len = length;
    }
}

void CoapPacket::make_uri_query(uint8_t* value, uint16_t length, char* largeBuf) {
    /*
    uint16_t split_position = 0;
    for( int i = 0; i < length; i++ )
    {
       if( value[i] == 61 ) // ascii of "="
       {
	  split_position = i;
	  break;
       }
    }
    query_t new_query;
    new_query.name = make_string( ( char* )value, split_position, largeBuf );
    new_query.value = make_string( ( char* )value + split_position + 1, length - split_position - 1, largeBuf );
    queries_.push_back( new_query );
     */
}

uint8_t CoapPacket::split_option(uint8_t opt, uint8_t current_delta, uint8_t* buf, char* seperator) {
    uint8_t index = 0;
    uint8_t buf_last = 0;
    uint8_t path_last = 0;
    uint8_t shift = 0;
    while (index < uri_path_len_) {
	index += strcspn(&uri_path_[index], seperator) + 1;
	if (index - path_last - 1 > 15) // large option
	{
	    buf[buf_last] = (opt - current_delta) << 4 | 15;
	    buf[buf_last + 1] = index - path_last - 1 - 15;
	    memcpy(&buf[buf_last + 2], &uri_path_[path_last], index - path_last - 1);
	    buf_last = index + (++shift);
	} else {
	    buf[buf_last] = (opt - current_delta) << 4 | (index - path_last - 1);
	    memcpy(&buf[buf_last + 1], &uri_path_[path_last], index - path_last - 1);
	    buf_last = index + shift;
	}
	path_last = index;
	current_delta = opt;
	opt_count_ += 1;
    }
    return buf_last;
}

uint8_t CoapPacket::power_of_two(uint16_t num) {
    uint8_t i = 0;
    while (num != 1) {
	num >>= 1;
	i++;
    }
    return i;
}

String CoapPacket::make_string(char* charArray, size_t charLen, char* largeBuf) {
    memcpy(largeBuf, charArray, charLen);
    largeBuf[charLen] = '\0';
    return String(largeBuf);
}

uint8_t CoapPacket::version_w() {
    return version_;
}

uint8_t CoapPacket::type_w() {
    return type_;
}

uint8_t CoapPacket::opt_count_w() {
    return opt_count_;
}

uint8_t CoapPacket::code_w() {
    return code_;
}

bool CoapPacket::isGET() {
    return code_w() == GET;
}

uint16_t CoapPacket::mid_w() {
    return mid_;
}

uint32_t CoapPacket::is_option(uint8_t opt) {
    return options_ & 1L << opt;
}

uint8_t CoapPacket::content_type_w() {
    return content_type_;
}

uint32_t CoapPacket::max_age_w() {
    return max_age_;
}

uint16_t CoapPacket::uri_host_w() {
    return uri_host_;
}

uint16_t CoapPacket::uri_port_w() {
    return uri_port_;
}

uint8_t CoapPacket::uri_path_len_w() {
    return uri_path_len_;
}

char* CoapPacket::uri_path_w() {
    return uri_path_;
}

uint16_t CoapPacket::observe_w() {
    return observe_;
}

uint8_t CoapPacket::token_len_w() {
    return token_len_;
}

uint8_t* CoapPacket::token_w() {
    return token_;
}

uint16_t CoapPacket::accept_w() {
    return accept_;
}

queries_t CoapPacket::uri_queries_w() {
    return queries_;
}

uint32_t CoapPacket::block2_num_w() {
    return block2_num_;
}

uint8_t CoapPacket::block2_more_w() {
    return block2_more_;
}

uint16_t CoapPacket::block2_size_w() {
    return block2_size_;
}

uint32_t CoapPacket::block2_offset_w() {
    return block2_offset_;
}

size_t CoapPacket::payload_len_w() {
    return payload_len_;
}

uint8_t* CoapPacket::payload_w() {
    return payload_;
}

void CoapPacket::set_version(uint8_t version) {
    version_ = version;
}

void CoapPacket::set_type(uint8_t type) {
    type_ = type;
}

void CoapPacket::set_opt_count(uint8_t opt_count) {
    opt_count_ = opt_count;
}

void CoapPacket::set_code(uint8_t code) {
    code_ = code;
}

void CoapPacket::set_mid(uint16_t mid) {
    mid_ = mid;
}

void CoapPacket::set_option(uint8_t opt) {
    options_ |= 1L << opt;
}

void CoapPacket::set_content_type(uint8_t content_type) {
    content_type_ = content_type;
}

void CoapPacket::set_max_age(uint32_t max_age) {
    max_age_ = max_age;
}

void CoapPacket::set_uri_host(uint16_t uri_host) {
    uri_host_ = uri_host;
}

void CoapPacket::set_uri_port(uint16_t uri_port) {
    uri_port_ = uri_port;
}

void CoapPacket::set_uri_path_len(uint8_t uri_path_len) {
    uri_path_len_ = uri_path_len;
}

void CoapPacket::set_uri_path(char* uri_path) {
    uri_path_ = uri_path;
}

void CoapPacket::set_observe(uint16_t observe) {
    observe_ = observe;
}

void CoapPacket::set_token_len(uint8_t token_len) {
    token_len_ = token_len;
}

void CoapPacket::set_token(uint8_t* token) {
    memcpy(token_, token, token_len_);
}

void CoapPacket::set_accept(uint16_t accept) {
    accept_ = accept;
}

void CoapPacket::set_uri_query(String uri_query_name, String uri_query_value) {
    /*
    query_t new_query;
    new_query.name = uri_query_name;
    new_query.value = uri_query_value;
    queries_.push_back( new_query );
     */
}

void CoapPacket::set_block2_num(uint32_t block2_num) {
    block2_num_ = block2_num;
}

void CoapPacket::set_block2_more(uint8_t block2_more) {
    block2_more_ = block2_more;
}

void CoapPacket::set_block2_size(uint16_t block2_size) {
    block2_size_ = block2_size;
}

void CoapPacket::set_block2_offset(uint32_t block2_offset) {
    block2_offset_ = block2_offset;
}

void CoapPacket::set_payload_len(uint8_t payload_len) {
    payload_len_ = payload_len;
}

void CoapPacket::set_payload(uint8_t* payload) {
    payload_ = payload;
}
