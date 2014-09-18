/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _FASTCDR_FASTCDR_DLL_H_
#define _FASTCDR_FASTCDR_DLL_H_

// normalize macros
#if !defined(FASTCDR_DYN_LINK) && !defined(FASTCDR_STATIC_LINK) \
    && !defined(EPROSIMA_ALL_DYN_LINK) && !defined(EPROSIMA_ALL_STATIC_LINK)
#define FASTCDR_STATIC_LINK
#endif

#if defined(EPROSIMA_ALL_DYN_LINK) && !defined(FASTCDR_DYN_LINK)
#define FASTCDR_DYN_LINK
#endif

#if defined(FASTCDR_DYN_LINK) && defined(FASTCDR_STATIC_LINK)
#error Must not define both FASTCDR_DYN_LINK and FASTCDR_STATIC_LINK
#endif

#if defined(EPROSIMA_ALL_NO_LIB) && !defined(FASTCDR_NO_LIB)
#define FASTCDR_NO_LIB
#endif

// enable dynamic linking

#if defined(_WIN32)
#if defined(EPROSIMA_ALL_DYN_LINK) || defined(FASTCDR_DYN_LINK)
#if defined(FASTCDR_SOURCE)
#define Cdr_DllAPI __declspec( dllexport )
#else
#define Cdr_DllAPI __declspec( dllimport )
#endif // FASTCDR_SOURCE
#else
#define Cdr_DllAPI
#endif
#else
#define Cdr_DllAPI
#endif // _WIN32

// Auto linking.

#if !defined(FASTCDR_SOURCE) && !defined(EPROSIMA_ALL_NO_LIB) \
    && !defined(FASTCDR_NO_LIB)

#include "fastcdr/FastCdr_version.h"

// Set properties.
#define EPROSIMA_LIB_NAME fastcdr

#if defined(EPROSIMA_ALL_DYN_LINK) || defined(FASTCDR_DYN_LINK)
#define EPROSIMA_DYN_LINK
#endif

#include "eProsima_cpp/config/eProsima_auto_link.h"
#endif // auto-linking disabled

#endif // _FASTCDR_FASTCDR_DLL_H_
