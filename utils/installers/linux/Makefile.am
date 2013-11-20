SUBDIRS = include

lib_LTLIBRARIES = libfastcdr.la
libfastcdr_la_SOURCES = src/cpp/Cdr.cpp \
			src/cpp/FastCdr.cpp \
			src/cpp/FastBuffer.cpp \
			src/cpp/exceptions/Exception.cpp \
			src/cpp/exceptions/NotEnoughMemoryException.cpp \
			src/cpp/exceptions/BadParamException.cpp

libfastcdr_la_LDFLAGS = -version-number VERSION_MAJOR:VERSION_MINOR:0

AM_CPPFLAGS = -std=c++0x -Iinclude

fastcdrdir = $(datadir)/fastcdr
fastcdr_DATA = FASTCDR_LIBRARY_LICENSE.txt LGPLv3_LICENSE.txt
