BASEDIR=.
OUTDIR= $(BASEDIR)/output
CC=gcc
CPP=g++
LN=g++
AR=ar
CP=cp
LNK=ln

all: fastcdr

-include $(BASEDIR)/thirdparty/dev-env/building/makefiles/eProsima.mk
-include $(BASEDIR)/building/makefiles/fastcdr.mk

.PHONY: fastcdr all check-env

fastcdr: check-env _fastcdr

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

check-env:
ifndef EPROSIMA_TARGET
	$(error EPROSIMA_TARGET environment variable has to be set)
endif

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif
