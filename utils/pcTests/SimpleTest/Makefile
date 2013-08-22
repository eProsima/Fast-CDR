BASEDIR=.
OUTDIR= $(BASEDIR)/output
CC=gcc
CPP=g++
LN=g++
AR=ar
CP=cp

-include $(EPROSIMADIR)/building/makefiles/eProsima.mk

SIMPLETEST_OUTDIR= $(OUTDIR)/SimpleTest
SIMPLETEST_OUTDIR_DEBUG = $(SIMPLETEST_OUTDIR)/debug
SIMPLETEST_OUTDIR_RELEASE = $(SIMPLETEST_OUTDIR)/release

SIMPLETEST_SED_OUTPUT_DIR_DEBUG= $(subst /,\\/,$(SIMPLETEST_OUTDIR_DEBUG))
SIMPLETEST_SED_OUTPUT_DIR_RELEASE= $(subst /,\\/,$(SIMPLETEST_OUTDIR_RELEASE))

SIMPLETEST_CFLAGS = $(CFLAGS) -std=c++0x
SIMPLETEST_CFLAGS_DEBUG = $(CFLAGS_DEBUG) -std=c++0x

SIMPLETEST_TARGET_DEBUG= $(BASEDIR)/bin/$(EPROSIMA_TARGET)/SimpleTestd
SIMPLETEST_TARGET= $(BASEDIR)/bin/$(EPROSIMA_TARGET)/SimpleTest

SIMPLETEST_INCLUDE_DIRS= $(INCLUDE_DIRS) -I$(BASEDIR)/../../../include \
                         -I$(EPROSIMADIR)/code

SIMPLETEST_LIBS_DEBUG= $(LIBS_DEBUG) -L$(BASEDIR)/../../../lib/$(EPROSIMA_TARGET) -lcdrd
SIMPLETEST_LIBS_RELEASE= $(LIBS) -L$(BASEDIR)/../../../lib/$(EPROSIMA_TARGET) -lcdr

SIMPLETEST_SRC_CFILES=

SIMPLETEST_SRC_CPPFILES= $(BASEDIR)/SimpleTest.cpp

# Project sources are copied to the current directory
SIMPLETEST_SRCS= $(SIMPLETEST_SRC_CFILES) $(SIMPLETEST_SRC_CPPFILES)

# Source directories
SIMPLETEST_SOURCES_DIRS_AUX= $(foreach srcdir, $(dir $(SIMPLETEST_SRCS)), $(srcdir))
SIMPLETEST_SOURCES_DIRS= $(shell echo $(SIMPLETEST_SOURCES_DIRS_AUX) | tr " " "\n" | sort | uniq | tr "\n" " ")

SIMPLETEST_OBJS_DEBUG = $(foreach obj,$(notdir $(addsuffix .o, $(SIMPLETEST_SRCS))), $(SIMPLETEST_OUTDIR_DEBUG)/$(obj))
SIMPLETEST_DEPS_DEBUG = $(foreach dep,$(notdir $(addsuffix .d, $(SIMPLETEST_SRCS))), $(SIMPLETEST_OUTDIR_DEBUG)/$(dep))
SIMPLETEST_OBJS_RELEASE = $(foreach obj,$(notdir $(addsuffix .o, $(SIMPLETEST_SRCS))), $(SIMPLETEST_OUTDIR_RELEASE)/$(obj))
SIMPLETEST_DEPS_RELEASE = $(foreach dep,$(notdir $(addsuffix .d, $(SIMPLETEST_SRCS))), $(SIMPLETEST_OUTDIR_RELEASE)/$(dep))

OBJS+= $(SIMPLETEST_OBJS_DEBUG) $(SIMPLETEST_OBJS_RELEASE)
DEPS+= $(SIMPLETEST_DEPS_DEBUG) $(SIMPLETEST_DEPS_RELEASE)

.PHONY: SimpleTest checkSimpleTestDirectories

SimpleTest: checkSimpleTestDirectories $(SIMPLETEST_TARGET_DEBUG) $(SIMPLETEST_TARGET)

checkSimpleTestDirectories:
	@mkdir -p $(OUTDIR)
	@mkdir -p $(SIMPLETEST_OUTDIR)
	@mkdir -p $(SIMPLETEST_OUTDIR_DEBUG)
	@mkdir -p $(SIMPLETEST_OUTDIR_RELEASE)
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/bin/$(EPROSIMA_TARGET)

$(SIMPLETEST_TARGET_DEBUG): $(SIMPLETEST_OBJS_DEBUG)
	$(LN) $(LDFLAGS) -o $(SIMPLETEST_TARGET_DEBUG) $(LIBRARY_PATH) $(SIMPLETEST_LIBS_DEBUG) $(SIMPLETEST_OBJS_DEBUG) $(SIMPLETEST_STATIC_LIBS)

$(SIMPLETEST_TARGET): $(SIMPLETEST_OBJS_RELEASE)
	$(LN) $(LDFLAGS) -o $(SIMPLETEST_TARGET) $(LIBRARY_PATH) $(SIMPLETEST_LIBS_RELEASE) $(SIMPLETEST_OBJS_RELEASE) $(SIMPLETEST_STATIC_LIBS)

vpath %.c $(SIMPLETEST_SOURCES_DIRS)
vpath %.cpp $(SIMPLETEST_SOURCES_DIRS)

$(SIMPLETEST_OUTDIR_DEBUG)/%.o:%.c
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS_DEBUG) -MM $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<  
	@$(CC) $(SIMPLETEST_CFLAGS_DEBUG) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

$(SIMPLETEST_OUTDIR_RELEASE)/%.o:%.c
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS) -MM $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

$(SIMPLETEST_OUTDIR_DEBUG)/%.c.o:%.c
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS_DEBUG) -MM $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<  
	@$(CC) $(SIMPLETEST_CFLAGS_DEBUG) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

$(SIMPLETEST_OUTDIR_RELEASE)/%.c.o:%.c
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS) -MM $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CC) $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

$(SIMPLETEST_OUTDIR_DEBUG)/%.cpp.o:%.cpp
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CPP) $(SIMPLETEST_CFLAGS_DEBUG) -MM $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<
	@$(CPP) $(SIMPLETEST_CFLAGS_DEBUG) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

$(SIMPLETEST_OUTDIR_RELEASE)/%.cpp.o:%.cpp
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CPP) $(SIMPLETEST_CFLAGS) -MM $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(SIMPLETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CPP) $(SIMPLETEST_CFLAGS) $(SIMPLETEST_INCLUDE_DIRS) $< -o $@

.PHONY: all

all: SimpleTest

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif
