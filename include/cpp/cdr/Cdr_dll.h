#ifndef _CPP_CDR_DLL_H_
#define _CPP_CDR_DLL_H_

// normalize macros
#if !defined(CDR_DYN_LINK) && !defined(CDR_STATIC_LINK) \
    && !defined(EPROSIMA_ALL_DYN_LINK) && !defined(EPROSIMA_ALL_STATIC_LINK)
#define CDR_STATIC_LINK
#endif

#if defined(EPROSIMA_ALL_DYN_LINK) && !defined(CDR_DYN_LINK)
#define CDR_DYN_LINK
#endif

#if defined(CDR_DYN_LINK) && defined(CDR_STATIC_LINK)
#error Must not define both CDR_DYN_LINK and CDR_STATIC_LINK
#endif

#if defined(EPROSIMA_ALL_NO_LIB) && !defined(CDR_NO_LIB)
#define CDR_NO_LIB
#endif

// enable dynamic linking

#if defined(_WIN32)
#if defined(EPROSIMA_ALL_DYN_LINK) || defined(CDR_DYN_LINK)
#if defined(CDR_SOURCE)
#define Cdr_DllAPI __declspec( dllexport )
#else
#define Cdr_DllAPI __declspec( dllimport )
#endif // CDR_SOURCE
#else
#define Cdr_DllAPI
#endif
#else
#define Cdr_DllAPI
#endif // _WIN32

// Auto linking.

#if !defined(CDR_SOURCE) && !defined(EPROSIMA_ALL_NO_LIB) \
    && !defined(CDR_NO_LIB)

#include "cpp/cdr/Cdr_version.h"

// Set properties.
#define EPROSIMA_LIB_NAME cdr

#if defined(EPROSIMA_ALL_DYN_LINK) || defined(CDR_DYN_LINK)
#define EPROSIMA_DYN_LINK
#endif

#include "eProsima_cpp/eProsima_auto_link.h"
#endif // auto-linking disabled

#endif // _CPP_CDR_DLL_H_
