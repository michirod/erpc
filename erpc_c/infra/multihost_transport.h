/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _EMBEDDED_RPC__MULTIHOST_TRANSPORT_H_
#define _EMBEDDED_RPC__MULTIHOST_TRANSPORT_H_

#include "erpc_common.h"
#include <cstring>
#include <stdint.h>

#define CLIENT_ID_TO_CORE_ID(_cid) (int16_t)((_cid) & 0xFFFF)
#define CLIENT_ID_TO_ADDR(_cid) (uint16_t)(((_cid) >> 16) & 0xFFFF)
#define CORE_ID_ADDR_TO_CLIENTID(_core, _addr) (((_core) & 0xFFFF) | (((_addr) & 0xFFFF) << 16))

/*!
 * @addtogroup infra_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
class MessageBuffer;

/*!
 * @brief Abstract interface for transport layer.
 *
 * @ingroup infra_transport
 */
class MultihostTransport
{
public:
    /*!
     * @brief Constructor.
     */
    MultihostTransport() {}

    /*!
     * @brief Transport destructor
     */
    virtual ~MultihostTransport() {}

    /*!
     * @brief Prototype for receiving message.
     *
     * Each transport layer need define this function.
     *
     * @param[out] message Will return pointer to received message buffer.
     * @param[out] client_id Will refer to client session.
     *
     * @return based on receive implementation.
     */
    virtual erpc_status_t select(MessageBuffer *message, int *client_id) = 0;

    /*!
     * @brief Prototype for send message.
     *
     * Each transport layer need define this function.
     *
     * @param[in] message Pass message buffer to send.
     * @param[in] client_id Specify destination client.
     *
     * @return based on send implementation.
     */
    virtual erpc_status_t send(const MessageBuffer *message, int client_id) = 0;

    /*!
     * @brief Poll for an incoming message.
     *
     * This function should return true if are some messages to process by server,
     * the return value should be tested before calling receive function to avoid
     * waiting for a new message (receive can be implemented as blocking function).
     *
     * @retval True when a message is available to process, else false.
     */
    virtual bool hasMessage() { return true; }

    /*!
     * @brief Asks for implementation specific address identifier
     *
     * @retval based on transport implementation
     */
    virtual uint32_t getAddr() = 0;

    /*!
     * @brief Asks for implementation specific protocol identifier
     *
     * @retval based on transport implementation
     */
    virtual uint32_t getProtocol() = 0;
};

/*!
 * @brief Abstract interface for transport factory.
 *
 * @ingroup infra_transport
 */
class MultihostTransportFactory
{
public:
    /*!
     * @brief Constructor.
     */
    MultihostTransportFactory() {}
    /*!
     * @brief TransportFactory destructor
     */
    virtual ~MultihostTransportFactory() {}
    /*!
    * @brief Return created transport object.
    *
    * @return Pointer to created transport object.
    */
    virtual MultihostTransport *create() = 0;
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__MULTIHOST_TRANSPORT_H_
