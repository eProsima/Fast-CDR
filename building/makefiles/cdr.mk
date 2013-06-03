CDR_OUTDIR= $(OUTDIR)/cdr
CDR_OUTDIR_DEBUG = $(CDR_OUTDIR)/debug
CDR_OUTDIR_RELEASE = $(CDR_OUTDIR)/release

CDR_SED_OUTPUT_DIR_DEBUG= $(subst /,\\/,$(CDR_OUTDIR_DEBUG))
CDR_SED_OUTPUT_DIR_RELEASE= $(subst /,\\/,$(CDR_OUTDIR_RELEASE))

CDR_TARGET_DEBUG= $(BASEDIR)/lib/$(EPROSIMA_TARGET)/libcdrd.so
CDR_TARGET_DEBUG_Z= $(BASEDIR)/lib/$(EPROSIMA_TARGET)/libcdrzd.a
CDR_TARGET= $(BASEDIR)/lib/$(EPROSIMA_TARGET)/libcdr.so
CDR_TARGET_Z= $(BASEDIR)/lib/$(EPROSIMA_TARGET)/libcdrz.a

CDR_CFLAGS += $(CFLAGS) -std=c++11
CDR_CFLAGS_DEBUG += $(CFLAGS_DEBUG) -std=c++11

CDR_INCLUDE_DIRS= $(INCLUDE_DIRS) -I$(BASEDIR)/include

CDR_SRC_CPPFILES= $(BASEDIR)/src/cpp/Cdr.cpp \
		  $(BASEDIR)/src/cpp/FastCdr.cpp \
		  $(BASEDIR)/src/cpp/FastBuffer.cpp \
		  $(BASEDIR)/src/cpp/exceptions/Exception.cpp \
		  $(BASEDIR)/src/cpp/exceptions/NotEnoughMemoryException.cpp \
		  $(BASEDIR)/src/cpp/exceptions/BadParamException.cpp

# Project sources are copied to the current directory
CDR_SRCS= $(CDR_SRC_CFILES) $(CDR_SRC_CPPFILES)

# Source directories
CDR_SOURCES_DIRS_AUX= $(foreach srcdir, $(dir $(CDR_SRCS)), $(srcdir))
CDR_SOURCES_DIRS= $(shell echo $(CDR_SOURCES_DIRS_AUX) | tr " " "\n" | sort | uniq | tr "\n" " ")

CDR_OBJS_DEBUG = $(foreach obj,$(notdir $(addsuffix .o, $(basename $(CDR_SRCS)))), $(CDR_OUTDIR_DEBUG)/$(obj))
CDR_DEPS_DEBUG = $(foreach dep,$(notdir $(addsuffix .d, $(basename $(CDR_SRCS)))), $(CDR_OUTDIR_DEBUG)/$(dep))
CDR_OBJS_RELEASE = $(foreach obj,$(notdir $(addsuffix .o, $(basename $(CDR_SRCS)))), $(CDR_OUTDIR_RELEASE)/$(obj))
CDR_DEPS_RELEASE = $(foreach dep,$(notdir $(addsuffix .d, $(basename $(CDR_SRCS)))), $(CDR_OUTDIR_RELEASE)/$(dep))

OBJS+= $(CDR_OBJS_DEBUG) $(CDR_OBJS_RELEASE)
DEPS+= $(CDR_DEPS_DEBUG) $(CDR_DEPS_RELEASE)

.PHONY: cdr checkCDRDirectories

cdr: checkCDRDirectories $(CDR_TARGET_DEBUG) $(CDR_TARGET_DEBUG_Z) $(CDR_TARGET) $(CDR_TARGET_Z)

checkCDRDirectories:
	@mkdir -p $(OUTDIR)
	@mkdir -p $(CDR_OUTDIR)
	@mkdir -p $(CDR_OUTDIR_DEBUG)
	@mkdir -p $(CDR_OUTDIR_RELEASE)

$(CDR_TARGET_DEBUG): $(CDR_OBJS_DEBUG)
	$(LN) $(LDFLAGS) -shared -o $(CDR_TARGET_DEBUG) $(LIBRARY_PATH) $(LIBS_DEBUG) $(CDR_OBJS_DEBUG)
	$(CP) $(CDR_TARGET_DEBUG) $(EPROSIMA_LIBRARY_PATH)/proyectos/$(EPROSIMA_TARGET)

$(CDR_TARGET_DEBUG_Z): $(CDR_OBJS_DEBUG)
	$(AR) -cru $(CDR_TARGET_DEBUG_Z) $(CDR_OBJS_DEBUG)
	$(CP) $(CDR_TARGET_DEBUG_Z) $(EPROSIMA_LIBRARY_PATH)/proyectos/$(EPROSIMA_TARGET)

$(CDR_TARGET): $(CDR_OBJS_RELEASE)
	$(LN) $(LDFLAGS) -shared -o $(CDR_TARGET) $(LIBRARY_PATH) $(LIBS) $(CDR_OBJS_RELEASE)
	$(CP) $(CDR_TARGET) $(EPROSIMA_LIBRARY_PATH)/proyectos/$(EPROSIMA_TARGET)

$(CDR_TARGET_Z): $(CDR_OBJS_RELEASE)
	$(AR) -cru $(CDR_TARGET_Z) $(CDR_OBJS_RELEASE)
	$(CP) $(CDR_TARGET_Z) $(EPROSIMA_LIBRARY_PATH)/proyectos/$(EPROSIMA_TARGET)

vpath %.cpp $(CDR_SOURCES_DIRS)

$(CDR_OUTDIR_DEBUG)/%.o:%.cpp
	@echo Calculating dependencies \(DEBUG mode\) $<
	@$(CPP) $(CDR_CFLAGS_DEBUG) -MM $(CDR_INCLUDE_DIRS) $< | sed "s/^.*:/$(CDR_SED_OUTPUT_DIR_DEBUG)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(DEBUG mode\) $<  
	$(CPP) $(CDR_CFLAGS_DEBUG) $(CDR_INCLUDE_DIRS) $< -o $@

$(CDR_OUTDIR_RELEASE)/%.o:%.cpp
	@echo Calculating dependencies \(RELEASE mode\) $<
	@$(CPP) $(CDR_CFLAGS) -MM $(CDR_INCLUDE_DIRS) $< | sed "s/^.*:/$(CDR_SED_OUTPUT_DIR_RELEASE)\/&/g" > $(@:%.o=%.d)
	@echo Compiling \(RELEASE mode\) $<
	$(CPP) $(CDR_CFLAGS) $(CDR_INCLUDE_DIRS) $< -o $@

