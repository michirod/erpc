mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ERPC_ROOT := ${mkfile_path}
APP_NAME := erpc

include ${mkfile_path}mk/erpc_common.mk

LIBERPCPREFIX ?= ${TARGET_OUTPUT_ROOT}

liberpc_out := ${TARGET_OUTPUT_ROOT}/lib/liberpc.a
liberpc_includes := ${TARGET_OUTPUT_ROOT}/include/erpc
liberpc_srcs := $(wildcard ${mkfile_path}erpc_c/*/*.cpp)

LIBERPC_CC := ${CC}
LIBERPC_CXX := ${CXX}
LIBERPC_AR := ${AR}
LIBERPC_LD := ${LD}

all:

${liberpc_includes}:
	make -C ${mkfile_path}erpc_c install_headers PREFIX=${LIBERPCPREFIX}

${liberpc_out}: ${liberpc_srcs} ${liberpc_includes}
	make -C ${mkfile_path} erpc AR="${LIBERPC_AR}" CC="${LIBERPC_CC}" CXX="${LIBERPC_CXX}" LD="${LIBERPC_LD}"

