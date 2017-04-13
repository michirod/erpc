/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#ifndef _EMBEDDED_RPC__SOCK_RPMSG_TRANSPORT_H_
#define _EMBEDDED_RPC__SOCK_RPMSG_TRANSPORT_H_

#include "message_buffer.h"
#include "static_queue.h"
#include "transport.h"

extern "C" {
#include <linux/rpmsg_socket.h>
#include <sys/socket.h>
#include <unistd.h>
}

/*!
 * @addtogroup rpmsg_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {
/*!
 * @brief Transport that uses sockRPMsg for interprocessor messaging.
 *
 * @ingroup sock_rpmsg_transport
 */
class sockRPMsgTransport : public Transport
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     */
    sockRPMsgTransport();

    /*!
     * @brief Codec destructor
     */
    virtual ~sockRPMsgTransport();

    /*!
     * @brief Initialization of sockRPMsgTransport layer.
     *
     * Call init() for sockRPMsg.
     *
     * @param[in] rem_port sockRPMsg remote port
     *
     * @retval kErpcStatus_Success When rpmsg init function was executed successfully.
     * @retval kErpcStatus_InitFailed When rpmsg init function wasn't executed successfully.
     */
    virtual erpc_status_t init(uint16_t port, uint16_t remote_vproc_id, bool serverRole);

    /*!
     * @brief Set message to first received message.
     *
     * In loop while no message come.
     *
     * @param[in] message Message buffer, to which will be stored incoming message.
     *
     * @retval kErpcStatus_Success if receive ok,
     * @retval kErpcStatus_ReceiveFailed if receive failed
     */
    virtual erpc_status_t receive(MessageBuffer *message);

    /*!
     * @brief Function to send prepared message.
     *
     * @param[in] message Pass message buffer to send.
     * @param[in] client_id Specify destination client.
     *
     * @retval kErpcStatus_InvalidArgument Wrong sendto arguments or message empty.
     * @retval kErpcStatus_ServerIsDown Peer unreachable
     * @retval kErpcStatus_SendFailed Failed to send message buffer.
     * @retval kErpcStatus_Success Successfully sent all data.
     */
    virtual erpc_status_t send(MessageBuffer *message);

protected:
    static int sock_fd;
    static uint16_t remote_port, remote_proc;
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__SOCK_RPMSG_TRANSPORT_H_
