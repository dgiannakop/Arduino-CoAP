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

// CURRENT COAP ATTRIBUTES
#define COAP_VERSION                        1
#define COAP_HEADER_VERSION_MASK            0xC0
#define COAP_HEADER_VERSION_SHIFT           6
#define COAP_HEADER_TYPE_MASK               0x30
#define COAP_HEADER_TYPE_SHIFT              4
#define COAP_HEADER_OPT_COUNT_MASK          0x0F
#define COAP_HEADER_OPT_COUNT_SHIFT         0
#define COAP_HEADER_LEN                     4
