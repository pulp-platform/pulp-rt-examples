PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g #-DFPGA

# This tells the platform to activate what is needed to connect the debug bridge
bridge=1
override CONFIG_OPT += camera/image-stream=$(CURDIR)/index.png
override CONFIG_OPT += runner/peripherals=true

ifdef RAW
override CONFIG_OPT += camera/color-mode=raw
PULP_CFLAGS += -DUSE_RAW=1
endif


include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
