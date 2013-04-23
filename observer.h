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

#ifndef OBSERVER_H
#define OBSERVER_H

typedef struct observer_t {
    uint16_t observe_id_;
    uint8_t observe_token_len_;
    uint16_t observe_last_mid_;
    CoapResource* observe_resource_;
    uint8_t observe_token_[8];
    unsigned long observe_timestamp_;
};

#endif 