# 
# Makefile
# 
# Copyright (C) 2015 ETH Zurich, University of Bologna
# All rights reserved.
# 

TEST_SRCS=hwme.c
PULP_APP = hwme
PULP_APP_SRCS = $(TEST_SRCS)
PULP_INC_PATHS += -DSDK
PULP_CFLAGS = -O2 -g

BUILD_DIR = $(CURDIR)/build/$(TEST_SRCS)

include $(PULP_SDK_HOME)/install/rules/pulp.mk

