mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ERPC_ROOT := ${mkfile_path}
include ${mkfile_path}mk/common.mk
include ${mkfile_path}mk/paths.mk
LIBERPCPREFIX ?= ${TARGET_OUTPUT_ROOT}

liberpc_out := ${TARGET_OUTPUT_ROOT}/lib/liberpc.a
liberpc_bin := ${LIBERPCPREFIX}/lib/liberpc.a
liberpc_srcs := $(wildcard ${mkfile_path}erpc_c/*/*.cpp)

all:

${liberpc_bin}: ${liberpc_out}
	make -C ${mkfile_path}erpc_c -e PREFIX=${LIBERPCPREFIX} install

${liberpc_out}: ${liberpc_srcs}
	env
	echo $+
	make -C ${mkfile_path} -e PREFIX=${LIBERPCPREFIX} erpc

liberpc.a: ${liberpc_bin}
