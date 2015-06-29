#ifndef _FASTCDR_CONFIG_H_
#define _FASTCDR_CONFIG_H_

#define VERSION_MAJOR @VERSION_MAJOR@
#define VERSION_MINOR @VERSION_MINOR@
#define VERSION_MICRO @VERSION_MICRO@

// C++11 support defines
#cmakedefine FASTCDR_SUPPORTS_CXX11
#cmakedefine FASTCDR_SUPPORTS_CXX01

// Endianness defines
#cmakedefine __BIG_ENDIAN__
#cmakedefine __LITTLE_ENDIAN__

#endif // _FASTCDR_CONFIG_H_
