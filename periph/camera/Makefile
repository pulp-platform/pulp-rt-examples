PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g

override CONFIG_OPT += camera/image-stream=$(CURDIR)/index.png
override CONFIG_OPT += runner/peripherals=true

USER_CONFIG = $(CURDIR)/config.ini

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
