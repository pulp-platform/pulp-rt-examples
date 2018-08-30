PULP_OMP_APP = test
# To be better optimized cluster code must be in a seperated file as some architectures
# has a different ISA between FC and cluster
PULP_APP_FC_SRCS = test.c
PULP_APP_CL_OMP_SRCS = cluster.c
PULP_CFLAGS = -O3 -g

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
