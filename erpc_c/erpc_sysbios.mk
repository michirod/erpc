mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

erpc_port_srcs := 	${mkfile_path}port/erpc_port_sysbios.cpp \
					${mkfile_path}port/erpc_threading_sysbios.cpp

include ${mkfile_path}erpc_rtos.mk