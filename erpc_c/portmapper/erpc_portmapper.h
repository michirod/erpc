/*
 * Generated by erpcgen 1.4.0 on Wed May  3 17:25:28 2017.
 *
 * AUTOGENERATED - DO NOT EDIT
 */

#if !defined(_erpc_portmapper_h_)
#define _erpc_portmapper_h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <erpc_transport_setup.h>

// Enumerators data types declarations
typedef enum pm_protocols
{
    DGRAM = 0,
    SEQPKT = 1,
    PROTOCOLS_COUNT = 2
} pm_protocols;

// Aliases data types declarations
typedef struct pm_entry pm_entry;
typedef struct list_0_t list_0_t;
typedef uint16_t pm_addr_port;
typedef int16_t pm_addr_addr;
typedef list_0_t pm_entry_list;

// Structures data types declarations
struct pm_entry
{
    uint32_t program_id;
    pm_protocols protocol;
    int16_t core_id;
    uint16_t port;
};

struct list_0_t
{
    pm_entry * elements;
    uint32_t elementsCount;
};

// Constant variable declarations
extern const pm_addr_port portmapper_port;

/*! @brief portmapper identifiers */
enum _portmapper_ids
{
    kportmapper_service_id = 1,
    kportmapper_register_service_id = 1,
    kportmapper_register_entry_id = 2,
    kportmapper_unregister_service_id = 3,
    kportmapper_unregister_entry_id = 4,
    kportmapper_lookup_id = 5,
    kportmapper_lookup_proto_id = 6,
    kportmapper_dump_id = 7,
};

#if defined(__cplusplus)
extern "C" {
#endif

//! @name portmapper
//@{
bool register_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port);

bool register_entry(erpc_transport_t transport, const pm_entry * entry);

bool unregister_service(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol, uint16_t port);

bool unregister_entry(erpc_transport_t transport, const pm_entry * entry);

pm_entry_list * lookup(erpc_transport_t transport, uint32_t program_id);

pm_entry_list * lookup_proto(erpc_transport_t transport, uint32_t program_id, pm_protocols protocol);

void dump(erpc_transport_t transport);
//@}

#if defined(__cplusplus)
}
#endif

#endif // _erpc_portmapper_h_