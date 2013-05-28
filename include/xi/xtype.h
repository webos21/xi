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

#ifndef _XTYPE_H_
#define _XTYPE_H_

/**
 * @brief XType Definitions
 *
 * @file xtype.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xcfgs.h"

/**
 * @defgroup xtype XType Definitions
 * @ingroup XI
 * @{
 */

/************************************************
 * Declaration Block
 * ----------------------------------------------
 * It is for saving the code of [extern "C"] block
 * for C Plus Plus.
 ************************************************/
#ifdef __cplusplus
#	define _XI_EXTERN_C_BEGIN  extern "C" {   ///< Block Start : Do not mangling the function name in c plus plus
#	define _XI_EXTERN_C_END    }              ///< Block End   : Do not mangling the function name in c plus plus
#else // !__cplusplus
#	define _XI_EXTERN_C_BEGIN                 ///< Block Start : it has no meaning on C
#	define _XI_EXTERN_C_END                   ///< Block End   : it has no meaning on C
#endif // __cplusplus


/************************************************
 * _XI_API_*
 * ----------------------------------------------
 * Generic helper definitions for shared library support
 ************************************************/
#ifdef _WIN32
#	ifdef _WINDLL
#		define _XI_API_PUBLIC __declspec(dllexport)   ///< Export the function name on WIN32
#	else
#		define _XI_API_PUBLIC __declspec(dllimport)   ///< Import the function name on WIN32
#	endif
#else // !_WIN32
#	define _XI_API_PUBLIC                             ///< It has no meaning on !WIN32
#endif // _WIN32

#if ((__GNUC__-0) * 10 + __GNUC_MINOR__-0 >= 33)
#	define _XI_API_INTERN __attribute__ ((visibility("hidden")))   ///< Do not show the function in binary
#	define _XI_API_UNUSED __attribute__ ((__unused__))             ///< Mark the function or variable is not used
#else // _GNUC_ < 3.3
#	define _XI_API_INTERN                                          ///< It has no meaning under GCC 3.3
#	define _XI_API_UNUSED                                          ///< It has no meaning under GCC 3.3
#endif // _GNUC_ >= 3.3


/************************************************
 * _XI_INLINE
 * ----------------------------------------------
 * The [inline] keyword abstraction for the independency
 * from the system private.
 ************************************************/
#ifdef __cplusplus
#define _XI_INLINE inline
#else // !__cplusplus
#	ifdef _MSC_VER
#		define _XI_INLINE __forceinline                                   ///< Mark the function is INLINE on WIN32
#	elif defined(__linux__) || defined(__MINGW32__)
#		define _XI_INLINE inline __attribute__((always_inline)) static    ///< Mark the function is INLINE on *NIX
#	else
#		define _XI_INLINE static                                          ///< Mark the function is INLINE on Others
#	endif
#endif // __cplusplus


/************************************************
 * _XI_CALLTYPE_*
 * ----------------------------------------------
 * The parameters are removed in the function.
 * @info __stdcall, __cdecl, __fastcall
 ************************************************/
#if defined(_WIN32) || defined(__i386__) || defined(__i686__)
#	ifdef _MSC_VER
#		define _XI_CALLTYPE_C     __cdecl                         ///< Mark the calling convention : cdecl
#		define _XI_CALLTYPE_STD   __stdcall                       ///< Mark the calling convention : standard
#		define _XI_CALLTYPE_FAST  __fastcall                      ///< Mark the calling convention : fast
#	else // !_MSC_VER
#		define _XI_CALLTYPE_C     __attribute__ ((cdecl))         ///< Mark the calling convention : cdecl
#		define _XI_CALLTYPE_STD   __attribute__ ((stdcall))       ///< Mark the calling convention : stdcall
#		define _XI_CALLTYPE_FAST  __attribute__ ((fastcall))      ///< Mark the calling convention : fastcall
#	endif // _MSC_VER
#else // !(_WIN32 || __i386__ || __i686__ )
#	define _XI_CALLTYPE_C                                         ///< It has no meaning on *NIX
#	define _XI_CALLTYPE_STD                                       ///< It has no meaning on *NIX
#	define _XI_CALLTYPE_FAST                                      ///< It has no meaning on *NIX
#endif // (_WIN32 || __i386__ || __i686__ )


/************************************************
 * NULL
 * ----------------------------------------------
 * If there is no NULL definition,
 * it defines NULL.
 ************************************************/
#ifndef NULL
#	ifdef __cplusplus
#		define NULL 0                ///< Define the NULL to 0 on c plus plus
#	else // !__cplusplus
#		define NULL ((void *)0)      ///< Define the NULL to null-pointer on C
#	endif // __cplusplus
#endif // NULL


/************************************************
 * Boolean (TRUE/FALSE)
 * ----------------------------------------------
 * If there is no TRUE/FALSE definition,
 * it defines TRUE/FALSE. (safe type for both c/c++)
 ************************************************/
#ifndef TRUE
#	define	TRUE  (1 == 1)    ///< Define the TRUE. It is a good definition to both c and c++.
#endif // TRUE
#ifndef FALSE
#	define	FALSE (1 != 1)    ///< Define the FALSE. It is a good definition to both c and c++.
#endif // FALSE


/************************************************
 * Unused Macro
 * ----------------------------------------------
 * Avoiding the warning "unused parameter"
 ************************************************/
#ifndef UNUSED
#	define	UNUSED(x) (void)(x)      ///< Define the [UNUSED] macro for avoiding warnings
#endif // UNUSED


/************************************************
 * Primitive Types Definition
 * ----------------------------------------------
 * References the inttypes.h
 * Both Win32 and Linux are equal.
 ************************************************/
typedef void               xvoid;    ///< define the type : void to xvoid

#ifdef __cplusplus
typedef bool               xbool;    ///< define the type : bool to xbool
#else // !__cplusplus
typedef unsigned char      xbool;    ///< define the type : unsigned char to xbool
#endif // __cplusplus
typedef char               xchar;    ///< define the type : char to xchar

typedef char               xint8;    ///< define the type : char to xint8
typedef unsigned char      xuint8;   ///< define the type : unsigned char to xuint8

typedef short              xint16;   ///< define the type : short to xint16
typedef unsigned short     xuint16;  ///< define the type : unsigned short to xuint16

typedef int                xint32;   ///< define the type : int to xint32
typedef unsigned int       xuint32;  ///< define the type : unsigned int to xuint32

typedef long long          xint64;   ///< define the type : long long to xint64
typedef unsigned long long xuint64;  ///< define the type : unsigned long long to xuint64

typedef float              xfloat32; ///< define the type : float to xfloat32
typedef double             xfloat64; ///< define the type : double to xfloat64


/************************************************
 * Define the xlong/xulong for synchronizing the size
 * ----------------------------------------------
 * Follow the size of *nix (64-bit LP data model)
 ************************************************/
#ifdef _WIN32
#    ifdef _WIN64
typedef long long          xlong;    ///< define the long long to xlong
typedef unsigned long long xulong;   ///< define the unsigned long long to xulong
#    else // !_WIN64
typedef long               xlong;    ///< define the long to xlong
typedef unsigned long      xulong;   ///< define the unsigned long to xulong
#    endif // _WIN64
#else // !_WIN32
typedef long               xlong;    ///< define the long to xlong
typedef unsigned long      xulong;   ///< define the unsigned long to xulong
#endif // _WIN32


/************************************************
 * Unicode Character Types Definition
 * ----------------------------------------------
 * Follow the *nix standard
 ************************************************/
typedef xuint16            xwchar;   ///< define the unsigned short integer to wide-character


/************************************************
 * Pointer Type Definition
 * ----------------------------------------------
 * for both 32-bit and 64-bit (LP Architecture)
 ************************************************/
typedef xlong              xintptr;
typedef xulong             xuintptr;


/************************************************
 * Size and Offset Type Definition
 * ----------------------------------------------
 * for both 32-bit and 64-bit (LP Architecture)
 ************************************************/
typedef xlong              xssize;   ///< define the xlong int to xssize
typedef xulong             xsize;    ///< define the xulong to xsize
#if defined(__x86_64) || defined(__amd64)
typedef xlong              xoff64;   ///< define the xint64 to xoff64
#else // !(defined(__x86_64) || defined(_amd_64))
typedef xint64             xoff64;   ///< define the xint64 to xoff64
#endif // defined(__x86_64) || defined(_amd_64)


/************************************************
 * FD Types Definition
 * ----------------------------------------------
 * Follow the *nix standard
 ************************************************/
typedef xint32             xfd;      ///< define the integer to file or socket descriptor


/************************************************
 * LIMITS
 * ----------------------------------------------
 * Minimum and Maximum values of TYPES
 ************************************************/
#define XI_SCHAR_MIN   (-128)                       ///< minimum signed char value
#define XI_SCHAR_MAX     127                        ///< maximum signed char value
#define XI_UCHAR_MAX     0xff                       ///< maximum unsigned char value
#define XI_CHAR_MIN    XI_SCHAR_MIN                 ///< mimimum char value
#define XI_CHAR_MAX    XI_SCHAR_MAX                 ///< maximum char value
#define XI_SHRT_MIN    (-32768)                     ///< minimum (signed) short value
#define XI_SHRT_MAX      32767                      ///< maximum (signed) short value
#define XI_USHRT_MAX     0xffff                     ///< maximum unsigned short value
#define XI_INT_MIN     (-2147483647 - 1)            ///< minimum (signed) int value
#define XI_INT_MAX       2147483647                 ///< maximum (signed) int value
#define XI_UINT_MAX      0xffffffff                 ///< maximum unsigned int value
#if defined(__x86_64) || defined(__amd64) || defined(_WIN64)
#define XI_LONG_MIN    (-9223372036854775807LL - 1) ///< minimum (signed) long value
#define XI_LONG_MAX      9223372036854775807LL      ///< maximum (signed) long value
#define XI_ULONG_MAX     0xffffffffffffffffULL      ///< maximum unsigned long value
#else // !(defined(_x86_64) || defined(__amd64) || defined(_WIN64))
#define XI_LONG_MIN    (-2147483647L - 1)           ///< minimum (signed) long value
#define XI_LONG_MAX      2147483647L                ///< maximum (signed) long value
#define XI_ULONG_MAX     0xffffffffUL               ///< maximum unsigned long value
#endif // defined(_x86_64) || defined(__amd64) || defined(_WIN64)
#define XI_LLONG_MAX     9223372036854775807LL      ///< maximum signed long long int value
#define XI_LLONG_MIN   (-9223372036854775807LL - 1) ///< minimum signed long long int value
#define XI_ULLONG_MAX    0xffffffffffffffffULL      ///< maximum unsigned long long int value


/************************************************
 * PATH
 * ----------------------------------------------
 * Platform specific path MACROS
 ************************************************/
#ifdef _WIN32
#	define XI_SEP_FILE_C  '\\'    ///< define the system dependent file separator character to '\\'
#	define XI_SEP_FILE_S  "\\"    ///< define the system dependent file separator string to  "\\"
#	define XI_SEP_PATH_C  ';'     ///< define the system dependent path separator character to ';'
#	define XI_SEP_PATH_S  ";"     ///< define the system dependent path separator string to  ";"
#else // !_WIN32
#	define XI_SEP_FILE_C  '/'     ///< define the system dependent file separator character to '/'
#	define XI_SEP_FILE_S  "/"     ///< define the system dependent file separator string to  "/"
#	define XI_SEP_PATH_C  ':'     ///< define the system dependent path separator character to ':'
#	define XI_SEP_PATH_S  ":"     ///< define the system dependent path separator string to  ":"
#endif // _WIN32

/**
* @}  // end of xtype
*/

#endif // _XTYPE_H_
