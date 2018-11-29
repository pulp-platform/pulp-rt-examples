PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g

override CONFIG_OPT += camera/image-stream=$(CURDIR)/index.pgm
override CONFIG_OPT += runner/peripherals=true


include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
