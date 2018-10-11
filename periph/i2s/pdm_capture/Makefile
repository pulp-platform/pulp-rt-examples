PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS ?= -O3 -g $(APP_CFLAGS)

override CONFIG_OPT += runner/peripherals=true

PULP_USER_CONFIG = $(CURDIR)/config.json

STIM_WAV ?= $(CONFIG_BUILD_DIR)/stim.wav
SAMPLING_FREQ ?= 44100

override CONFIG_OPT += microphone/stim_left=$(STIM_WAV) microphone/pdm=true

override PULP_CFLAGS += -O3 -g -DSAMPLING_FREQ=$(SAMPLING_FREQ)

gen:
	sox -n -r $(SAMPLING_FREQ) $(STIM_WAV) synth 3 sine 500

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
