#ifndef __khrplatform_h_
#define __khrplatform_h_

/*
** Copyright (c) 2008-2018 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* Khronos platform-specific types and definitions.
 *
 * The master copy of khrplatform.h is maintained in the Khronos EGL
 * Registry repository at https://github.com/KhronosGroup/EGL-Registry
 * The last semantic modification to khrplatform.h was at commit ID:
 *      67a3e0864c2d75ea5287b9f3d2eb74a745936692
 *
 * Adopters may modify this file to suit their platform. Adopters are
 * encouraged to submit platform specific modifications to the Khronos
 * group so that they can be included in future versions of this file.
 * Please submit changes by filing pull requests or issues on
 * the EGL Registry repository linked above.
 *
 *
 * See the Implementer's Guidelines for information about where this file
 * should be located on your system and for more details of its use:
 *    http://www.khronos.org/registry/implementers_guide.pdf
 *
 * This file should be included as
 *        #include <KHR/khrplatform.h>
 * by Khronos client API header files that use its types and defines.
 *
 * The types in khrplatform.h should only be used to define API-specific types.
 *
 * Types defined in khrplatform.h:
 *    khronos_int8_t              signed   8  bit
 *    khronos_uint8_t             unsigned 8  bit
 *    khronos_int16_t             signed   16 bit
 *    khronos_uint16_t            unsigned 16 bit
 *    khronos_int32_t             signed   32 bit
 *    khronos_uint32_t            unsigned 32 bit
 *    khronos_int64_t             signed   64 bit
 *    khronos_uint64_t            unsigned 64 bit
 *    khronos_intptr_t            signed   same number of bits as a pointer
 *    khronos_uintptr_t           unsigned same number of bits as a pointer
 *    khronos_ssize_t             signed   size
 *    khronos_usize_t             unsigned size
 *    khronos_float_t             signed   32 bit floating point
 *    khronos_time_ns_t           unsigned 64 bit time in nanoseconds
 *    khronos_utime_nanoseconds_t unsigned time interval or absolute time in
 *                                         nanoseconds
 *    khronos_stime_nanoseconds_t signed time interval in nanoseconds
 *    khronos_boolean_enum_t      enumerated boolean type. This should
 *      only be used as a base type when a client API's boolean type is
 *      an enum. Client APIs which use an integer or other type for
 *      booleans cannot use this as the base type for their boolean.
 *
 * Tokens defined in khrplatform.h:
 *
 *    KHRONOS_FALSE, KHRONOS_TRUE Enumerated boolean false/true values.
 *
 *    KHRONOS_SUPPORT_INT64 is 1 if 64 bit integers are supported; otherwise 0.
 *    KHRONOS_SUPPORT_FLOAT is 1 if floats are supported; otherwise 0.
 *
 * Calling convention macros defined in this file:
 *    KHRONOS_APICALL
 *    KHRONOS_APIENTRY
 *    KHRONOS_APIATTRIBUTES
 *
 * These may be used in function prototypes as:
 *
 *      KHRONOS_APICALL void KHRONOS_APIENTRY funcname(
 *                                  int arg1,
 *                                  int arg2) KHRONOS_APIATTRIBUTES;
 */

#if defined(__SCITECH_SNAP__)
/* Older Watcom versions are missing the intptr_t types... */
typedef          int          khronos_intptr_t;
typedef unsigned int          khronos_uintptr_t;
typedef          long long    khronos_int64_t;
typedef unsigned long long    khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif defined(_WIN32) && !defined(__GNUC__)
/* Win32 system with MSVC */
typedef          __int8       khronos_int8_t;
typedef unsigned __int8       khronos_uint8_t;
typedef          __int16      khronos_int16_t;
typedef unsigned __int16      khronos_uint16_t;
typedef          __int32      khronos_int32_t;
typedef unsigned __int32      khronos_uint32_t;
typedef          __int64      khronos_int64_t;
typedef unsigned __int64      khronos_uint64_t;
#ifdef _WIN64
typedef          __int64      khronos_intptr_t;
typedef unsigned __int64      khronos_uintptr_t;
typedef          __int64      khronos_ssize_t;
typedef unsigned __int64      khronos_usize_t;
#else
typedef          __int32      khronos_intptr_t;
typedef unsigned __int32      khronos_uintptr_t;
typedef          __int32      khronos_ssize_t;
typedef unsigned __int32      khronos_usize_t;
#endif
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#elif defined(__linux__) || defined(__unix__)
/* Generic Unix */
#include <stdint.h>
#include <sys/types.h>  /* for ssize_t and size_t */
typedef int8_t                khronos_int8_t;
typedef uint8_t               khronos_uint8_t;
typedef int16_t               khronos_int16_t;
typedef uint16_t              khronos_uint16_t;
typedef int32_t               khronos_int32_t;
typedef uint32_t              khronos_uint32_t;
typedef int64_t               khronos_int64_t;
typedef uint64_t              khronos_uint64_t;
typedef intptr_t              khronos_intptr_t;
typedef uintptr_t             khronos_uintptr_t;
typedef ssize_t               khronos_ssize_t;
typedef size_t                khronos_usize_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#else
/* Fallback for other platforms */
#include <stdint.h>
typedef int8_t                khronos_int8_t;
typedef uint8_t               khronos_uint8_t;
typedef int16_t               khronos_int16_t;
typedef uint16_t              khronos_uint16_t;
typedef int32_t               khronos_int32_t;
typedef uint32_t              khronos_uint32_t;
typedef int64_t               khronos_int64_t;
typedef uint64_t              khronos_uint64_t;
typedef intptr_t              khronos_intptr_t;
typedef uintptr_t             khronos_uintptr_t;
typedef ssize_t               khronos_ssize_t;
typedef size_t                khronos_usize_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1
#endif

/*
 * Types that are (so far) the same on all platforms
 */
typedef float                 khronos_float_t;
typedef khronos_uint64_t      khronos_time_ns_t;
typedef khronos_uint64_t      khronos_utime_nanoseconds_t;
typedef khronos_int64_t       khronos_stime_nanoseconds_t;

/*
 * Dummy value used to pad enum types to 32 bits.
 */
#ifndef KHRONOS_MAX_ENUM
#define KHRONOS_MAX_ENUM 0x7FFFFFFF
#endif

/*
 * Enumerated boolean type
 *
 * Values other than zero should be considered to be true.  Therefore
 * comparisons should not be made against KHRONOS_TRUE.
 */
typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = KHRONOS_MAX_ENUM
} khronos_boolean_enum_t;

/*
 * Function calling conventions
 */
#if defined(_WIN32)
#   if defined(_MSC_VER)
#       define KHRONOS_APICALL __declspec(dllimport)
#       define KHRONOS_APIENTRY __stdcall
#       define KHRONOS_APIATTRIBUTES
#   elif defined(__MINGW32__)
#       define KHRONOS_APICALL __declspec(dllimport)
#       define KHRONOS_APIENTRY __stdcall
#       define KHRONOS_APIATTRIBUTES
#   else
#       define KHRONOS_APICALL
#       define KHRONOS_APIENTRY __stdcall
#       define KHRONOS_APIATTRIBUTES
#   endif
#elif defined(__ANDROID__)
#   define KHRONOS_APICALL __attribute__((visibility("default")))
#   define KHRONOS_APIENTRY
#   define KHRONOS_APIATTRIBUTES
#elif (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303) \
        || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
/* KHRONOS_APIATTRIBUTES is not used by the client API headers yet */
#   define KHRONOS_APICALL __attribute__((visibility("default")))
#   define KHRONOS_APIENTRY
#   define KHRONOS_APIATTRIBUTES
#else
#   define KHRONOS_APICALL
#   define KHRONOS_APIENTRY
#   define KHRONOS_APIATTRIBUTES
#endif

#endif /* __khrplatform_h_ */
