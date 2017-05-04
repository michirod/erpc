#include "erpc_portmapper_setup.h"
#include "erpc_port.h"
#include "erpc_client_setup.h"

#if !(__embedded_cplusplus)
using namespace std;
#endif

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// global client variables
//static ManuallyConstructed<ClientManager> s_client;
//ClientManager *g_client = NULL;
//static ManuallyConstructed<BasicCodecFactory> s_codecFactory;

erpc_transport_t pm_transport;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////


void erpc_portmapper_init(erpc_transport_t transport, erpc_mbf_t message_buffer_factory){
    erpc_client_init(transport, message_buffer_factory);
    pm_transport = transport;
}

pm_entry_list * erpc_portmapper_query(unsigned int program_id){
    return lookup(pm_transport, (uint32_t) program_id);
}

pm_entry_list * erpc_portmapper_query_proto(unsigned int program_id, pm_protocols protocol){
    return lookup_proto(pm_transport, (uint32_t) program_id, protocol);
}

void erpc_portmapper_free_list(pm_entry_list *list){
    erpc_free(list->elements);
    erpc_free(list);
}

void erpc_portmapper_deinit(void)
{}
