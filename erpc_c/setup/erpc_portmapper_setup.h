#ifndef _EMBEDDED_RPC__PORTMAPPER_SETUP_H_
#define _EMBEDDED_RPC__PORTMAPPER_SETUP_H_

#include "erpc_common.h"
#include "erpc_mbf_setup.h"
#include "erpc_portmapper.h"

/*!
 * @addtogroup server_setup
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//! @name Portmapper setup
//@{

/*!
 * @brief This function initializes portmapper client.
 *
 * This function initializes portmapper client with all components necessary for serve client request.
 */
void erpc_portmapper_init(erpc_transport_t transport, erpc_mbf_t message_buffer_factory);

///*!
// * @brief This function creates a transport for a specific program
// *
// * @param[in] program_id The program_id of the desierd service
// *
// * @retval An already configured transport for the first available server.
// */
//erpc_transport_t erpc_portmapper_setup_p(unsigned int program_id);

///*!
// * @brief This function creates a transport for a specific program
// *
// * @param[in] program_id The program_id of the desierd service
// * @param[in] protocol The protocol to use for communications
// *
// * @retval An already configured transport for the first available server usign desired protocol.
// */
//erpc_transport_t erpc_portmapper_setup_pp(unsigned int program_id, pm_protocols protocol);

///*!
// * @brief This function creates a transport for a specific program
// *
// * @param[in] program_id The program_id of the desierd service
// * @param[in] core_id The core_id of the desired server providing the service
// *
// * @retval An already configured transport for the specified server.
// */
//erpc_transport_t erpc_portmapper_setup_pc(unsigned int program_id, pm_addr_addr core_id);

///*!
// * @brief This function creates a transport for a specific program
// *
// * @param[in] program_id The program_id of the desierd service
// * @param[in] protocol The protocol to use for communications
// * @param[in] core_id The core_id of the desired server providing the service
// *
// * @retval An already configured transport for the specified server using desired protocol.
// */
//erpc_transport_t erpc_portmapper_setup_ppc(unsigned int program_id, pm_protocols protocol, pm_addr_addr core_id);

/*!
 * @brief This function queries the portmapper for a desired program
 *
 * @param[in] program_id The program_id of the desierd service
 *
 * @retval A structure containing a list of pm_entries describing servers servicing
 *          the desired program (must be explicitly freed with erpc_portmapper_free_list).
 */
pm_entry_list * erpc_portmapper_query(unsigned int program_id);

/*!
 * @brief This function queries the portmapper for a desired program using a specified protocol
 *
 * @param[in] program_id The program_id of the desierd service
 * @param[in] protocol The protocol to use for communications
 *
 * @retval A structure containing a list of pm_entries describing servers servicing
 *          the desired program (must be explicitly freed with erpc_portmapper_free_list).
 */
pm_entry_list * erpc_portmapper_query_proto(unsigned int program_id, pm_protocols protocol);

/*!
 * @brief This function cleans up memory used by query calls
 *
 * @param[in] list A list of pm_entries
 */
void erpc_portmapper_free_list(pm_entry_list *list);

/*!
 * @brief This function de-initializes portmapper client.
 *
 * This function de-initializes portmapper client and all components which it own.
 */
void erpc_portmapper_deinit(void);

//@}

#ifdef __cplusplus
}
#endif

/*! @} */


#endif //_EMBEDDED_RPC__PORTMAPPER_SETUP_H_
