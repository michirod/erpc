#include "erpc_portmapper.h"

#include <linux/rpmsg_socket.h>
#include <sys/socket.h>

#include "erpc_port.h"
#include "erpc_portmapper_server.h"
#include "erpc_multihost_server_setup.h"
#include "erpc_transport_setup.h"

#include <map>
#include <list>
#include <string.h>

#define fill_pm_entry(_e, _prog, _proto, _r, _cnt) ({ \
    _e[_cnt].program_id = _prog; \
    _e[_cnt].protocol = _proto; \
    _e[_cnt].core_id = _r->core_id; \
    _e[_cnt].port = _r->port; \
    _cnt++; \
})

/*********************************
 * Types and globals definitions
 *********************************/

typedef struct {
    int16_t core_id;
    uint16_t port;
} remote_entry;

typedef std::list< remote_entry* > pm_entry_clist;
typedef std::map< uint32_t, pm_entry_clist* > pm_map_t;

pm_map_t pm_map;

/*******************************
 * Portmapper server functions
 *******************************/

bool pm_register_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port){
    pm_entry_clist *entries;
    pm_map_t::iterator mit;
    pm_entry_clist::iterator lit;
    bool good = true;
    int16_t core_id = (int16_t) ((int)transport & 0xFFFF);

    printf("registering service %d on port %d\n", program_id, port);

    if(protocol >= PROTOCOLS_COUNT){
        return false;
    }

    mit = pm_map.find(program_id);
    if(mit == pm_map.end()){
        pm_entry_clist * protocol_entries = new pm_entry_clist[PROTOCOLS_COUNT];
        pm_map[program_id] = protocol_entries;
        entries = &(protocol_entries[protocol]);
    } else {
        entries = &(pm_map[program_id][protocol]);
    }

    for(lit = entries->begin(); lit != entries->end(); lit ++){
        if((*lit)->core_id == core_id && (*lit)->port == port){
            good = false;
            break;
        }
    }

    if(good){
        remote_entry *remote = (remote_entry *) erpc_malloc(sizeof(remote_entry));
        remote->core_id = core_id;
        remote->port = port;
        entries->push_front(remote);
    }

    return good;
}

bool pm_unregister_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port){
    pm_entry_clist *entries;
    pm_map_t::iterator mit;
    pm_entry_clist::iterator lit;
    bool good = false;
    int16_t core_id = (int16_t) ((int)transport & 0xFFFF);

    if(protocol >= PROTOCOLS_COUNT){
        return false;
    }

    mit = pm_map.find(program_id);
    if(mit == pm_map.end()){
        return false;
    } else {
        entries = &(pm_map[program_id][protocol]);
    }

    for(lit = entries->begin(); lit != entries->end(); lit ++){
        if((*lit)->core_id == core_id && (*lit)->port == port){
            good = true;
            break;
        }
    }

    if(good){
        remote_entry *remote = *lit;
        entries->remove(remote);
        free(remote);
        int totalsize = 0, i;

        for(i = 0; i < PROTOCOLS_COUNT; i++){
            totalsize += pm_map[program_id][i].size();
        }
        if(totalsize <= 0){
            pm_map.erase(pm_map.find(program_id));
        }
    }

    return good;
}



pm_entry_list * pm_lookup(uint32_t program_id, pm_protocols *protocol){

    if(protocol != NULL && (*protocol) >= PROTOCOLS_COUNT){
        return NULL;
    }

    pm_entry_clist * entries;
    pm_map_t::iterator mit;
    mit = pm_map.find(program_id);
    if(mit == pm_map.end()){
        return NULL;
    } else {
        entries = pm_map[program_id];
    }

    int total_hits = 0;
    if(protocol == NULL){
        for(unsigned int i = 0; i < PROTOCOLS_COUNT; i++){
            total_hits += entries[i].size();
        }
    } else {
        total_hits = entries[*protocol].size();
    }

    pm_entry_list * out_list = (pm_entry_list *) erpc_malloc(sizeof(pm_entry_list));

    out_list->elementsCount = total_hits;
    if(total_hits > 0) {
        out_list->elements = (pm_entry *) erpc_malloc(total_hits * sizeof(pm_entry));
        pm_entry * ptr = out_list->elements;

        pm_entry_clist::iterator lit;
        unsigned int el_cnt = 0;

        if(protocol == NULL){
            for(unsigned int i = 0; i < PROTOCOLS_COUNT; i++){
                for(lit = entries[i].begin(); lit != entries[i].end(); lit ++){
                    fill_pm_entry(ptr, program_id, (pm_protocols) i, (*lit), el_cnt);
                }
            }
        } else {
            for(lit = entries[*protocol].begin(); lit != entries[*protocol].end(); lit ++){
                fill_pm_entry(ptr, program_id, (*protocol), (*lit), el_cnt);
            }
        }
    } else {
        out_list->elements = NULL;
    }

    return out_list;
}

void pm_dump(){
    unsigned int i;

    pm_entry_clist::iterator lit;

    printf("Dumping portmapper table:\n"
           "------------------------------------\n"
           "progr , proto , core  , port\n");

    for(auto mit : pm_map){
        for(i = 0; i < PROTOCOLS_COUNT; i++){
            for(lit = mit.second[i].begin(); lit != mit.second[i].end(); lit ++){
                printf("%d\t%d\t%d\t%d\n", mit.first, i, (*lit)->core_id, (*lit)->port);
            }
        }
    }

    printf("------------------------------------\n"
           "\n");
}


/********************
 * C code interface
 ********************/

#if defined(__cplusplus)
extern "C" {
#endif

bool register_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port){
    return pm_register_service(transport, program_id, protocol, port);
}

bool register_entry(erpc_transport_t transport, const pm_entry * entry){
    return pm_register_service(transport, entry->program_id, entry->protocol, entry->port);
}

bool unregister_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port){
    return pm_unregister_service(transport, program_id, protocol, port);
}

bool unregister_entry(erpc_transport_t transport, const pm_entry * entry){
    return pm_unregister_service(transport, entry->program_id, entry->protocol, entry->port);
}

pm_entry_list * lookup(erpc_transport_t transport, uint32_t program_id){
    (void) (transport);
    return pm_lookup(program_id, NULL);
}

pm_entry_list * lookup_proto(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol){
    (void) (transport);
    return pm_lookup(program_id, &protocol);
}

void dump(erpc_transport_t transport){
    (void) (transport);
    pm_dump();
}

#if defined(__cplusplus)
}
#endif

#ifdef ERPC_PORTMAPPER_STANDALONE

/************************
 * Main runner function
 ************************/

int main(int argc, char ** argv){
    int error = 0;

    printf("starting portmapper server...\n");

    erpc_transport_t transport = erpc_transport_sock_rpmsg_multihost_init(portmapper_port, 0, true);
    if(transport == NULL){
        printf("Error: Unable to bind server socket\n");
        return kErpcStatus_InitFailed;
    }
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();
    erpc_multihost_server_init(transport, NULL, message_buffer_factory);

    erpc_add_service_to_multihost_server(create_portmapper_service());

    printf("portmapper ready, listening on port %d (DGRAM)\n", portmapper_port);

    error = erpc_multihost_server_run();

    if(error){
        printf("Error: %d", error);
    } else {
        printf("server was shut down");
    }

    printf("server quitting");

    erpc_multihost_server_deinit();
    return error;
}

#endif
