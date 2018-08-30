PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g #-DFPGA

# This tells the platform to activate what is needed to connect the debug bridge
bridge=1
override CONFIG_OPT += camera/image-stream=$(CURDIR)/imgTest%d.pgm
override CONFIG_OPT += runner/peripherals=true

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
