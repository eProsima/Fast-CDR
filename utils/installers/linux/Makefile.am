SUBDIRS = include

lib_LTLIBRARIES = libfastcdr.la
libfastcdr_la_SOURCES = src/cpp/Cdr.cpp \
			src/cpp/FastCdr.cpp \
			src/cpp/FastBuffer.cpp \
			src/cpp/exceptions/Exception.cpp \
			src/cpp/exceptions/NotEnoughMemoryException.cpp \
			src/cpp/exceptions/BadParamException.cpp

libfastcdr_la_LDFLAGS = -version-number VERSION_MAJOR:VERSION_MINOR:VERSION_RELEASE

AM_CPPFLAGS = -std=c++0x -Iinclude

fastcdr_licenciasdir = $(datadir)/doc/fastcdr
fastcdr_licencias_DATA = FASTCDR_LIBRARY_LICENSE.txt LGPLv3_LICENSE.txt
