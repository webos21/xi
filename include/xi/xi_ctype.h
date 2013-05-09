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

#ifndef _XI_CTYPE_H_
#define _XI_CTYPE_H_

/**
 * @brief XI Character Type API
 *
 * @file xi_ctype.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_ctype Character Type API
 * @ingroup XI
 * @{
 */

/**
 * Checks for an alphanumeric character.
 *
 * @param c the single character to check
 */
xint32   xi_isalnum(xint32 c);

/**
 * Checks for an alphabetic character
 *
 * @param c the single character to check
 */
xint32   xi_isalpha(xint32 c);

/**
 * Checks whether c is a 7-bit unsigned char value that fits into the ASCII character set.
 *
 * @param c the single character to check
 */
xint32   xi_isascii(xint32 c);

/**
 * Checks for a control character.
 *
 * @param c the single character to check
 */
xint32   xi_iscntrl(xint32 c);

/**
 * Checks for a digit (0 through 9).
 *
 * @param c the single character to check
 */
xint32   xi_isdigit(xint32 c);

/**
 * Checks for any printable character except space.
 *
 * @param c the single character to check
 */
xint32   xi_isgraph(xint32 c);

/**
 * Checks for a lower-case character.
 *
 * @param c the single character to check
 */
xint32   xi_islower(xint32 c);

/**
 * Checks for any printable character including space.
 *
 * @param c the single character to check
 */
xint32   xi_isprint(xint32 c);

/**
 * Checks for any printable character which is not a space or an alphanumeric character.
 *
 * @param c the single character to check
 */
xint32   xi_ispunct(xint32 c);

/**
 * checks for white-space characters.
 *
 * In the "C" and "POSIX" locales, these are: space, form-feed ('\\f'), newline ('\\n'), carriage return ('\\r'),
 * horizontal tab ('\\t'), and vertical tab ('\\v').
 *
 * @param c the single character to check
 */
xint32   xi_isspace(xint32 c);

/**
 * Checks for an uppercase letter.
 *
 * @param c the single character to check
 */
xint32   xi_isupper(xint32 c);

/**
 * Checks for a hexadecimal digits, that is,
 * one of (0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F).
 *
 * @param c the single character to check
 */
xint32   xi_isxdigit(xint32 c);

/**
 * Converts c to a 7-bit unsigned char value that fits into the ASCII character set,
 * by clearing the high-order bits.
 *
 * @param c the single character to check
 */
xint32   xi_toascii(xint32 c);

/**
 * Converts the letter c to lower case, if possible.
 *
 * @param c the single character to check
 */
xint32   xi_tolower(xint32 c);

/**
 * Converts the letter c to upper case, if possible.
 *
 * @param c the single character to check
 */
xint32   xi_toupper(xint32 c);

/**
 * @}  // end of xi_ctype
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_CTYPE_H_
