
this_makefile := $(firstword $(MAKEFILE_LIST))
ERPC_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)
include $(ERPC_ROOT)/mk/erpc_common.mk
TARGET_OUTPUT_ROOT = $(OUTPUT_ROOT)/$(DEBUG_OR_RELEASE)/$(os_name)/$(APP_NAME)
#-----------------------------------------------
# setup variables
# ----------------------------------------------

APP_NAME = portmapper

DEFINES += -DERPC_PORTMAPPER_STANDALONE

LIBRARIES += ${TARGET_OUTPUT_ROOT}/../erpc/lib/liberpc.a

#-----------------------------------------------
# Include path. Add the include paths like this:
# INCLUDES += ./include/
#-----------------------------------------------
INCLUDES += $(ERPC_ROOT)/erpc_c/config \
			$(ERPC_ROOT)/erpc_c/infra \
			$(ERPC_ROOT)/erpc_c/port \
			$(ERPC_ROOT)/erpc_c/portmapper \
			$(ERPC_ROOT)/erpc_c/setup \
			$(ERPC_ROOT)/erpc_c/transports

SOURCES += $(ERPC_ROOT)/portmapper_server/erpc_portmapper_server.cpp \
		   $(ERPC_ROOT)/portmapper_server/erpc_portmapper_server_runner.cpp

HEADERS += $(ERPC_ROOT)/portmapper_server/erpc_portmapper_server.h \
		   $(ERPC_ROOT)/erpc_c/portmapper/erpc_portmapper.h

include $(ERPC_ROOT)/mk/targets.mk

.PHONY: install
install: $(MAKE_TARGET)
	@$(call printmessage,c,Installing, erpcgen in $(BIN_INSTALL_DIR))
	$(at)mkdir -p $(BIN_INSTALL_DIR)
	$(at)install $(MAKE_TARGET) $(BIN_INSTALL_DIR)

clean::
	$(at)rm -rf $(OBJS_ROOT)/*.cpp $(OBJS_ROOT)/*.hpp $(OBJS_ROOT)/*.c
