/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XI_BASE64_H_
#define _XI_BASE64_H_

/**
 * @brief XI Base64 encoding/decoding API
 *
 * @file xi_base64.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_base64 Base64 encoding/decoding API
 * @ingroup XI
 * @{
 * @brief
 * Simple BASE64 encode/decode functions.\n
 * \n
 * As we might encode binary strings, hence we require the length of
 * the incoming plain source. And return the length of what we decoded.\n
 * \n
 * The decoding function takes any non valid char (i.e. whitespace, \\0
 * or anything non A-Z,0-9 etc) as terminal.\n
 * \n
 * The handling of terminating \\0 characters differs from function to
 * function.
 *
 */

/**
 * Given the length of an un-encrypted string, get the length of the
 * encrypted string.
 *
 * @param len the length of an unencrypted string.
 * @return the length of the string after it is encrypted, including the trailing \\0
 */
xssize xi_base64_encode_len(xsize len);


/**
 * Encode a text string using base64encoding.
 *
 * @param coded_dst The destination string for the encoded string. A \\0 is appended.
 * @param plain_src The original string in plain text
 * @param len_plain_src The length of the plain text string
 * @return the length of the encoded string, including the trailing \\0
 */
xssize xi_base64_encode(xchar * coded_dst, const xchar *plain_src,
		xsize len_plain_src);


/**
 * Determine the maximum buffer length required to decode the plain text
 * string given the encoded string.
 *
 * @param coded_src The encoded string
 * @return the maximum required buffer length for the plain text string
 */
xssize xi_base64_decode_len(const xchar * coded_src);


/**
 * Decode a string to plain text.
 *
 * @param plain_dst The destination string for the plain text. A \\0 is appended.
 * @param coded_src The encoded string
 * @return the length of the plain text string (excluding the trailing \\0)
 */
xssize xi_base64_decode(xchar * plain_dst, const xchar *coded_src);


/**
 * Decode an string to plain text. This is the same as xi_base64_decode()
 * except the result is not \\0-terminated
 *
 * @param plain_dst The destination string for the plain text. The string is not \\0-terminated.
 * @param coded_src The encoded string
 * @return the length of the plain text string
 */
xssize xi_base64_decode_binary(xuint8 * plain_dst, const xchar *coded_src);


/**
 * @}  // end of xi_base64
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_BASE64_H_
