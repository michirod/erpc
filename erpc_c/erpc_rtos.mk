mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

erpc_config_srcs += 

erpc_config_hdts += $(patsubst %.cpp,%.h,${erpc_config_srcs}) \
					${mkfile_path}config/erpc_config.h


erpc_infra_srcs +=	${mkfile_path}infra/basic_codec.cpp \
					${mkfile_path}infra/client_manager.cpp \
					${mkfile_path}infra/message_buffer.cpp \
					${mkfile_path}infra/multihost_server.cpp \
					${mkfile_path}infra/server.cpp \
					${mkfile_path}infra/simple_server.cpp

erpc_infra_hdrs +=	$(patsubst %.cpp,%.h,${erpc_infra_srcs}) \
					${mkfile_path}infra/codec.h \
					${mkfile_path}infra/erpc_common.h \
					${mkfile_path}infra/erpc_version.h \
					${mkfile_path}infra/manually_constructed.h \
					${mkfile_path}infra/multihost_transport.h \
					${mkfile_path}infra/static_queue.h \
					${mkfile_path}infra/transport.h


erpc_port_srcs += 	

erpc_port_hdrs +=	$(patsubst %.cpp,%.h,${erpc_port_srcs}) \
					${mkfile_path}port/erpc_config_internal.h \
					${mkfile_path}port/erpc_port.h \
					${mkfile_path}port/erpc_threading.h


erpc_portmapper_srcs += ${mkfile_path}portmapper/erpc_portmapper_client.cpp

erpc_portmapper_hdrs += ${mkfile_path}portmapper/erpc_portmapper.h


erpc_setup_srcs += 	${mkfile_path}setup/erpc_client_setup.cpp \
					${mkfile_path}setup/erpc_multihost_server_setup.cpp \
					${mkfile_path}setup/erpc_server_setup.cpp \
					${mkfile_path}setup/erpc_setup_mbf_dynamic.cpp \
					${mkfile_path}setup/erpc_setup_sock_rpmsg_multihost_rtos.cpp \
					${mkfile_path}setup/erpc_setup_sock_rpmsg_rtos.cpp 

erpc_setup_hdrs += 	$(patsubst %.cpp,%.h,${erpc_setup_srcs}) \
					${mkfile_path}setup/erpc_mbf_setup.h \
					${mkfile_path}setup/erpc_setup.h \
					${mkfile_path}setup/erpc_transport_setup.h


erpc_transports_srcs += ${mkfile_path}transports/sock_rpmsg_multihost_rtos_transport.cpp \
						${mkfile_path}transports/sock_rpmsg_rtos_transport.cpp

erpc_transports_hdrs += $(patsubst %.cpp,%.h,${erpc_transport_srcs}) \


erpc_srcs = ${erpc_config_srcs} ${erpc_infra_srcs} ${erpc_port_srcs} ${erpc_portmapper_srcs} ${erpc_setup_srcs} ${erpc_transports_srcs}
erpc_hdrs = ${erpc_config_hdrs} ${erpc_infra_hdrs} ${erpc_port_hdrs} ${erpc_portmapper_hdrs} ${erpc_setup_hdrs} ${erpc_transports_hdrs}
erpc_includes = ${mkfile_path}config ${mkfile_path}infra ${mkfile_path}port ${mkfile_path}portmapper ${mkfile_path}setup ${mkfile_path}transports

