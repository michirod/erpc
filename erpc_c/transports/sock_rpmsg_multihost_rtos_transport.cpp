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
 * LOSS OF USE, DATA, OR PROFI65TS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sock_rpmsg_multihost_rtos_transport.h"
#include <new>

#include "rpmsg_socket.h"
#include "rpmsg_channels.h"
#include "rpmsg_al.h"
#include <errno.h>

#if !(__embedded_cplusplus)
using namespace std;
#endif

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

sockRPMsgMultihostRTOSTransport::sockRPMsgMultihostRTOSTransport()
: MultihostTransport(),
  sock_fd(-1),
  server_role(false),
  server_protocol(DGRAM)
{
}

sockRPMsgMultihostRTOSTransport::~sockRPMsgMultihostRTOSTransport()
{
    rpmsg_socket_close(sock_fd);
}

uint32_t sockRPMsgMultihostRTOSTransport::getAddr(){
    return (uint32_t) (server_port);
}

uint32_t sockRPMsgMultihostRTOSTransport::getProtocol(){
    return (uint32_t) (server_protocol);
}

erpc_status_t sockRPMsgMultihostRTOSTransport::init(uint16_t port, bool serverRole)
{
    int ret_value, new_fd;
    struct sockaddr_rpmsg serveraddr;

    serveraddr.family = AF_RPMSG;
    serveraddr.vproc_id = 0;

    new_fd = rpmsg_socket_create(RPMSG_ST_DGRAM);
    if(new_fd < 0){
        return kErpcStatus_InitFailed;
    }

    server_role = serverRole;

    if(serverRole){
        server_port = serveraddr.addr = port;
        ret_value = rpmsg_socket_bind(new_fd, &serveraddr);
        if(ret_value < 0){
            rpmsg_socket_close(new_fd);
            fl_printf("CPU1: ERROR binding socket %d: %d\r\n", serveraddr.addr, ret_value);
            return kErpcStatus_InitFailed;
        }
    } else {
        return kErpcStatus_InvalidArgument;
    }

    sock_fd = new_fd;

    return kErpcStatus_Success;
}

erpc_status_t sockRPMsgMultihostRTOSTransport::select(MessageBuffer *message, int *client_id)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    sockaddr_rpmsg remote_addr;

    uint32_t length = message->getLength();
    uint8_t *freeBuffer = message->get();

    ret_value = rpmsg_socket_recvfrom(sock_fd, freeBuffer, length, &remote_addr);
    if(ret_value < 0){
        fl_printf("CPU1: ERROR receiving message: %d\r\n", ret_value);
        return kErpcStatus_ReceiveFailed;
    }

    (*client_id) = (remote_addr.vproc_id & 0xFFFF) | (remote_addr.addr & 0xFFFF) << 16;

    return kErpcStatus_Success;
}

erpc_status_t sockRPMsgMultihostRTOSTransport::send(const MessageBuffer *message, int client_id)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    sockaddr_rpmsg remote_addr;

    remote_addr.addr = (client_id >> 16) & 0xFFFF;
    remote_addr.family = AF_RPMSG;
    remote_addr.vproc_id = client_id & 0xFFFF;

    ret_value = rpmsg_socket_sendto(sock_fd, (void *)message->get(), message->getUsed(), &remote_addr);
    if (ret_value < 0) {
        perror("Error");
        switch (ret_value) {
        case -EINVAL:
        case -EMSGSIZE:
        case -ENOTSUP:
            return kErpcStatus_InvalidArgument;
        case -EHOSTUNREACH:
            return kErpcStatus_ServerIsDown;
        default:
            return kErpcStatus_SendFailed;
        }
    }

    return kErpcStatus_Success;
}
