SUBDIRS = include

lib_LTLIBRARIES = libcdr.la
libcdr_la_SOURCES = src/cpp/Cdr.cpp \
			src/cpp/FastCdr.cpp \
			src/cpp/FastBuffer.cpp \
			src/cpp/exceptions/Exception.cpp \
			src/cpp/exceptions/NotEnoughMemoryException.cpp \
			src/cpp/exceptions/BadParamException.cpp

libcdr_la_LDFLAGS = -version-number VERSION_MAJOR:VERSION_MINOR:0

AM_CPPFLAGS = -std=c++0x -Iinclude

cdrdir = $(datadir)/cdr
cdr_DATA = FASTCDR_LIBRARY_LICENSE.txt LGPLv3_LICENSE.txt
