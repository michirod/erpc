mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

erpc_port_srcs := 	${mkfile_path}port/erpc_port_freertos.cpp \
					${mkfile_path}port/erpc_threading_freertos.cpp

include ${mkfile_path}erpc_rtos.mk