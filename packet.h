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

#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <coaptypedef.h>
#include <coap_conf.h>

class CoapPacket {
public:
    void init(void);
    coap_status_t buffer_to_packet(uint8_t len, uint8_t* buf, char* helperBuf);
    uint8_t packet_to_buffer(uint8_t* buf);
    // get
    uint8_t version_w();
    uint8_t type_w();
    uint8_t opt_count_w();
    uint8_t code_w();
    bool isGET();
    bool isPOST();
    uint16_t mid_w();
    uint32_t is_option(uint8_t opt);
    uint8_t content_type_w();
    uint32_t max_age_w();
    uint16_t uri_host_w();
    uint16_t uri_port_w();
    uint8_t uri_path_len_w();
    char* uri_path_w();
    uint16_t observe_w();
    uint8_t token_len_w();
    uint8_t* token_w();
    uint16_t accept_w();
    queries_t uri_queries_w();
    uint32_t block2_num_w();
    uint8_t block2_more_w();
    uint16_t block2_size_w();
    uint32_t block2_offset_w();
    size_t payload_len_w();
    uint8_t* payload_w();
    // set
    void set_version(uint8_t version);
    void set_type(uint8_t type);
    void set_opt_count(uint8_t opt_count);
    void set_code(uint8_t code);
    void set_mid(uint16_t mid);
    void set_option(uint8_t opt);
    void set_content_type(uint8_t content_type);
    void set_max_age(uint32_t max_age);
    void set_uri_host(uint16_t uri_host);
    void set_uri_port(uint16_t uri_port);
    void set_uri_path_len(uint8_t uri_path_len);
    void set_uri_path(char* uri_path);
    void set_observe(uint16_t observe);
    void set_token_len(uint8_t token_len);
    void set_token(uint8_t* token);
    void set_accept(uint16_t accept);
    void set_uri_query(String uri_query_name, String uri_query_value);
    void set_block2_num(uint32_t block2_num);
    void set_block2_more(uint8_t block2_more);
    void set_block2_size(uint16_t block2_size);
    void set_block2_offset(uint32_t block2_offset);
    void set_payload_len(uint8_t payload_len);
    void set_payload(uint8_t* payload);
protected:
    uint8_t add_fence_opt(uint8_t opt, uint8_t *current_delta, uint8_t *buf);
    uint8_t set_opt_header(uint8_t delta, size_t len, uint8_t *buf);
    uint8_t set_int_opt_value(uint8_t opt, uint8_t current_delta, uint8_t *buf, uint32_t value);
    uint16_t get_int_opt_value(uint8_t *value, uint16_t length, bool hexAscii);
    void merge_options(char **dst, size_t *dst_len, uint8_t *value, uint16_t length, char seperator);
    void make_uri_query(uint8_t* value, uint16_t length, char* largeBuf);
    uint8_t split_option(uint8_t opt, uint8_t current_delta, uint8_t* buf, char* seperator);
    uint8_t power_of_two(uint16_t num);
    String make_string(char* charArray, size_t charLen, char* largeBuf);
private:
    uint8_t version_;
    uint8_t type_;
    uint8_t opt_count_;
    uint8_t code_;
    uint16_t mid_;

    uint32_t options_;

    uint8_t content_type_; // 1
    uint32_t max_age_; // 2
    //TODO...
    //size_t proxy_uri_len_; // 3
    //char *proxy_uri_; // 3
    //uint8_t etag_len_; // 4
    //uint8_t etag[8]_; // 4
    //size_t uri_host_len_; // 5
    uint16_t uri_host_; // 5
    uint16_t uri_port_; // 7
    //TODO...
    size_t uri_path_len_; // 9
    char *uri_path_; // 9
    uint16_t observe_; // 10
    uint8_t token_len_; // 11
    uint8_t token_[8]; // 11
    uint16_t accept_; // 12
    //TODO...
    //uint8_t if_match_len_; // 13
    //uint8_t if_match_[8]; // 13
    queries_t queries_; // 15

    // block2 17
    uint32_t block2_num_; // 17
    uint8_t block2_more_; // 17
    uint16_t block2_size_; // 17
    uint32_t block2_offset_; // 17
    //uint32_t block1_num_; // 19
    //uint8_t block1_more_; // 19
    //uint16_t block1_size_; // 19
    //uint32_t block1_offset_; // 19
    //uint8_t if_none_match; // 21

    uint8_t payload_len_;
    uint8_t *payload_;
};
#endif
