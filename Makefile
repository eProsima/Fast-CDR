BASEDIR=.
OUTDIR= $(BASEDIR)/output
CC=gcc
CPP=g++
LN=g++
AR=ar
CP=cp
LNK=ln

-include $(BASEDIR)/thirdparty/eProsima/building/makefiles/eProsima.mk
-include $(BASEDIR)/building/makefiles/fastcdr.mk

.PHONY: all

all: fastcdr

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif
