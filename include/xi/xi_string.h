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

#ifndef _XI_STRING_H_
#define _XI_STRING_H_

/**
 * @brief XI String API
 *
 * @file xi_string.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_string String API
 * @ingroup XI
 * @{
 */

/**
 * Calculate the length of a string.
 * Not including the terminating \\0 character.
 *
 * @param s The string to know the length
 * @return The length of the string
 */
xsize    xi_strlen(const xchar *s);


/**
 * Concatenate two strings
 * The strings may not overlap, and the dest string must have
 * enough space for the result.
 *
 * @param dest The destination string
 * @param src The source string to concatenate
 * @return The new string (dest string pointer)
 */
xchar   *xi_strcat(xchar *dest, const xchar *src);


/**
 * Concatenate two strings to n-bytes of src
 * If src contains n or more characters, strncat() writes n+1 characters to dest
 * (n from src plus the terminating null byte).
 * Therefore, the size of dest must be at least strlen(dest)+n+1.
 *
 * @param dest The destination string
 * @param src The source string to concatenate
 * @param n The length to concatenate
 * @return The new string (dest string pointer)
 */
xchar   *xi_strncat(xchar *dest, const xchar *src, xsize n);


/**
 * Copy a string
 * The strings may not overlap, and the destination string dest must be
 * large enough to receive the copy.
 *
 * @param dest The destination string
 * @param src The source string to concatenate
 * @return The new string (dest string pointer)
 */
xchar   *xi_strcpy(xchar *dest, const xchar *src);


/**
 * Copy a string to n-bytes of src
 * If the length of src is less than n, strncpy() pads the remainder of dest with null bytes.
 *
 * @param dest The destination string
 * @param src The source string to concatenate
 * @param n The length to concatenate
 * @return The new string (dest string pointer)
 */
xchar   *xi_strncpy(xchar *dest, const xchar *src, xsize n);


/**
 * Query a pointer to the first occurrence of the character c
 * in the string s.
 *
 * @param s The string to be retrieved
 * @param c The character to find
 * @return a pointer to the matched character or NULL
 *         if the character is not found.
 */
xchar   *xi_strchr(const xchar *s, xint32 c);


/**
 * Query a pointer to the last occurrence of the character c
 * in the string s.
 *
 * @param s The string to be retrieved
 * @param c The character to find
 * @return a pointer to the matched character or NULL
 *         if the character is not found.
 */
xchar   *xi_strrchr(const xchar *s, xint32 c);


/**
 * Finds the first occurrence of the substring k in the string s.
 *
 * @param s The string to be retrieved
 * @param k The substring to find
 * @return a pointer to the beginning of the substring or NULL
 *         if the substring is not found.
 */
xchar   *xi_strstr(const xchar *s, const xchar *k);


/**
 * Split a string into separate null-terminated tokens.  The tokens are
 * delimited in the string by one or more characters from the delim
 * argument.
 *
 * @param s The string to separate; this should be specified on the
 *            first call to xi_strtok() for a given string, and NULL
 *            on subsequent calls.
 * @param delim The set of delimiters
 * @param ptrptr Internal state saved by xi_strtok() between calls.
 * @return The next token from the string
 */
xchar   *xi_strtok(xchar *s, const xchar *delim, xchar **ptrptr);


/**
 * Do a natural order comparison of two strings.
 *
 * @param s1 The first string to compare
 * @param s2 The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
xint32   xi_strcmp(const xchar *s1, const xchar *s2);


/**
 * Do a natural order comparison of two strings to n-bytes.
 *
 * @param s1 The first string to compare
 * @param s2 The second string to compare
 * @param n The length to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
xint32   xi_strncmp(const xchar *s1, const xchar *s2, xsize n);


/**
 * Do a natural order comparison of two strings ignoring the case of the
 * strings.
 *
 * @param s1 The first string to compare
 * @param s2 The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
xint32   xi_strcasecmp(const xchar *s1, const xchar *s2);


/**
 * Do a natural order comparison of two strings ignoring the case of the
 * strings to n-bytes.
 *
 * @param s1 The first string to compare
 * @param s2 The second string to compare
 * @param n The length to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
xint32   xi_strncasecmp(const xchar *s1, const xchar *s2, xsize n);


/**
 * sprintf routine
 *
 * @param str The buffer to write to
 * @param format The format string
 * @param ... The arguments to use to fill out the format string.
 */
xint32   xi_sprintf(xchar *str, const xchar *format, ...);


/**
 * snprintf routine
 *
 * @param str The buffer to write to
 * @param size The size of the buffer
 * @param format The format string
 * @param ... The arguments to use to fill out the format string.
 */
xint32   xi_snprintf(xchar *str, xsize size, const xchar *format, ...);


/**
 * parse a numeric string into a 32-bit numeric value
 *
 * @param str The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param endptr A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 * @param base A numeric base in the range between 2 and 36 inclusive,
 *   or 0.  If base is zero, buf will be treated as base ten unless its
 *   digits are prefixed with '0x', in which case it will be treated as
 *   base 16.
 * @return The numeric value of the string.  On overflow, errno is set
 *   to ERANGE.  On success, errno is set to 0.
 */
xint32   xi_strtoi(const xchar *str, xchar **endptr, xint32 base);


/**
 * parse a numeric string into a 64-bit numeric value
 *
 * @param str The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param endptr A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 * @param base A numeric base in the range between 2 and 36 inclusive,
 *   or 0.  If base is zero, buf will be treated as base ten unless its
 *   digits are prefixed with '0x', in which case it will be treated as
 *   base 16.
 * @return The numeric value of the string.  On overflow, errno is set
 *   to ERANGE.  On success, errno is set to 0.
 */
xint64   xi_strtoi64(const xchar *str, xchar **endptr, xint32 base);


/**
 * Convert a numeric string into a 32-bit float value
 *
 * @param str The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param endptr A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 */
xfloat32 xi_strtof(const xchar *str, xchar **endptr);


/**
 * Convert a numeric string into a 64-bit float value
 *
 * @param str The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param endptr A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 */
xfloat64 xi_strtof64(const xchar *str, xchar **endptr);

/**
 * @}  // end of xi_string
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_STRING_H_
