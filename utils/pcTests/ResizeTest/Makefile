BASEDIR=.
OUTDIR= $(BASEDIR)/output
CC=gcc
CPP=g++
LN=g++
AR=ar
CP=cp

-include $(BASEDIR)/../../../thirdparty/eProsima/building/makefiles/eProsima.mk

RESIZETEST_OUTDIR= $(OUTDIR)/ResizeTest
RESIZETEST_OUTDIR_DEBUG = $(RESIZETEST_OUTDIR)/debug
RESIZETEST_OUTDIR_RELEASE = $(RESIZETEST_OUTDIR)/release

RESIZETEST_SED_OUTPUT_DIR_DEBUG= $(subst /,\\/,$(RESIZETEST_OUTDIR_DEBUG))
RESIZETEST_SED_OUTPUT_DIR_RELEASE= $(subst /,\\/,$(RESIZETEST_OUTDIR_RELEASE))

RESIZETEST_CFLAGS = $(CFLAGS) -std=c++0x
RESIZETEST_CFLAGS_DEBUG = $(CFLAGS_DEBUG) -std=c++0x

RESIZETEST_TARGET_DEBUG= $(BASEDIR)/bin/$(EPROSIMA_TARGET)/ResizeTestd
RESIZETEST_TARGET= $(BASEDIR)/bin/$(EPROSIMA_TARGET)/ResizeTest

RESIZETEST_INCLUDE_DIRS= $(INCLUDE_DIRS) -I$(BASEDIR)/../../../include \
                         -I$(BASEDIR)/../../../thirdparty/eProsima/code

RESIZETEST_LIBS_DEBUG= $(LIBS_DEBUG) -L$(BASEDIR)/../../../lib/$(EPROSIMA_TARGET) -lfastcdrd
RESIZETEST_LIBS_RELEASE= $(LIBS) -L$(BASEDIR)/../../../lib/$(EPROSIMA_TARGET) -lfastcdr

RESIZETEST_SRC_CFILES=

RESIZETEST_SRC_CPPFILES= $(BASEDIR)/ResizeTest.cpp

# Project sources are copied to the current directory
RESIZETEST_SRCS= $(RESIZETEST_SRC_CFILES) $(RESIZETEST_SRC_CPPFILES)

# Source directories
RESIZETEST_SOURCES_DIRS_AUX= $(foreach srcdir, $(dir $(RESIZETEST_SRCS)), $(srcdir))
RESIZETEST_SOURCES_DIRS= $(shell echo $(RESIZETEST_SOURCES_DIRS_AUX) | tr " " "\n" | sort | uniq | tr "\n" " ")

RESIZETEST_OBJS_DEBUG = $(foreach obj,$(notdir $(addsuffix .o, $(RESIZETEST_SRCS))), $(RESIZETEST_OUTDIR_DEBUG)/$(obj))
RESIZETEST_DEPS_DEBUG = $(foreach dep,$(notdir $(addsuffix .d, $(RESIZETEST_SRCS))), $(RESIZETEST_OUTDIR_DEBUG)/$(dep))
RESIZETEST_OBJS_RELEASE = $(foreach obj,$(notdir $(addsuffix .o, $(RESIZETEST_SRCS))), $(RESIZETEST_OUTDIR_RELEASE)/$(obj))
RESIZETEST_DEPS_RELEASE = $(foreach dep,$(notdir $(addsuffix .d, $(RESIZETEST_SRCS))), $(RESIZETEST_OUTDIR_RELEASE)/$(dep))

OBJS+= $(RESIZETEST_OBJS_DEBUG) $(RESIZETEST_OBJS_RELEASE)
DEPS+= $(RESIZETEST_DEPS_DEBUG) $(RESIZETEST_DEPS_RELEASE)

.PHONY: ResizeTest checkResizeTestDirectories

ResizeTest: checkResizeTestDirectories $(RESIZETEST_TARGET_DEBUG) $(RESIZETEST_TARGET)

checkResizeTestDirectories:
	@mkdir -p $(OUTDIR)
	@mkdir -p $(RESIZETEST_OUTDIR)
	@mkdir -p $(RESIZETEST_OUTDIR_DEBUG)
	@mkdir -p $(RESIZETEST_OUTDIR_RELEASE)
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/bin/$(EPROSIMA_TARGET)

$(RESIZETEST_TARGET_DEBUG): $(RESIZETEST_OBJS_DEBUG)
	$(LN) $(LDFLAGS) -o $(RESIZETEST_TARGET_DEBUG) $(LIBRARY_PATH) $(RESIZETEST_LIBS_DEBUG) $(RESIZETEST_OBJS_DEBUG) $(RESIZETEST_STATIC_LIBS)

$(RESIZETEST_TARGET): $(RESIZETEST_OBJS_RELEASE)
	$(LN) $(LDFLAGS) -o $(RESIZETEST_TARGET) $(LIBRARY_PATH) $(RESIZETEST_LIBS_RELEASE) $(RESIZETEST_OBJS_RELEASE) $(RESIZETEST_STATIC_LIBS)

vpath %.c $(RESIZETEST_SOURCES_DIRS)
vpath %.cpp $(RESIZETEST_SOURCES_DIRS)

$(RESIZETEST_OUTDIR_DEBUG)/%.o:%.c
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS_DEBUG) -MM $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<  
	@$(CC) $(RESIZETEST_CFLAGS_DEBUG) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

$(RESIZETEST_OUTDIR_RELEASE)/%.o:%.c
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS) -MM $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

$(RESIZETEST_OUTDIR_DEBUG)/%.c.o:%.c
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS_DEBUG) -MM $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<  
	@$(CC) $(RESIZETEST_CFLAGS_DEBUG) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

$(RESIZETEST_OUTDIR_RELEASE)/%.c.o:%.c
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS) -MM $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CC) $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

$(RESIZETEST_OUTDIR_DEBUG)/%.cpp.o:%.cpp
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CPP) $(RESIZETEST_CFLAGS_DEBUG) -MM $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<
	@$(CPP) $(RESIZETEST_CFLAGS_DEBUG) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

$(RESIZETEST_OUTDIR_RELEASE)/%.cpp.o:%.cpp
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CPP) $(RESIZETEST_CFLAGS) -MM $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< | sed "s/^.*:/$(RESIZETEST_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	@$(CPP) $(RESIZETEST_CFLAGS) $(RESIZETEST_INCLUDE_DIRS) $< -o $@

.PHONY: all

all: ResizeTest

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif
