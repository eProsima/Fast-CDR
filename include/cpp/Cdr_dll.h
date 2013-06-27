#ifndef _CDR_DLL_H_
#define _CDR_DLL_H_

#include "eProsima_cpp/eProsima_cpp_dll.h"

#if defined(_WIN32)
#if defined(CDR_DLL_EXPORT)
#define Cdr_DllAPI __declspec( dllexport )
#else
#define Cdr_DllAPI __declspec( dllimport )
#endif /* CDR_EXPORT */
#endif /* _WIN32 */

#endif // _CDR_DLL_H_
