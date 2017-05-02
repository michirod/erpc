/*
 * Generated by erpcgen 1.4.0 on Tue May  2 12:52:34 2017.
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
extern const uint16_t portmapper_port;

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
};

#if defined(__cplusplus)
extern "C" {
#endif

//! @name portmapper
//@{
bool register_service(uint32_t program_id, pm_protocols protocol, int16_t core_id, uint16_t port);

bool register_entry(const pm_entry * entry);

bool unregister_service(uint32_t program_id, pm_protocols protocol, int16_t core_id, uint16_t port);

bool unregister_entry(const pm_entry * entry);

pm_entry_list * lookup(uint32_t program_id);

pm_entry_list * lookup_proto(uint32_t program_id, pm_protocols protocol);
//@}

#if defined(__cplusplus)
}
#endif

#endif // _erpc_portmapper_h_
