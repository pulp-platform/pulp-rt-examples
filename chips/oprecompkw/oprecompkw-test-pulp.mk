#
# Copyright (C) 2019 TU Kaiserslautern
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
#

TOPDIR := $(dir $(lastword $(MAKEFILE_LIST)))

PULP_APP ?= test
PULP_APP_FC_SRCS ?= test.c
PULP_CFLAGS += -O3 -g -Wno-main

PULP_PROPERTIES += pulp_chip
include $(PULP_SDK_HOME)/install/rules/pulp_properties.mk

# Set a local dependency for coherence checks
run:: check_target
build:: check_target

target_chip = oprecompkw
ifeq ($(pulp_chip),$(target_chip))
check_target:
	$(info Running test for $(pulp_chip))
else
check_target:
	$(error Bad configuration. This test is intended to $(target_chip). Current config is $(pulp_chip))
endif

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk

clean:: local_clean

local_clean:
	@echo Removing build folder
	rm -rf build
