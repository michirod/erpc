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

#include "sock_rpmsg_rtos_transport.h"
#include <new>

#include "rpmsg_socket.h"
#include "rpmsg_channels.h"
#include "rpmsg_al.h"

#if !(__embedded_cplusplus)
using namespace std;
#endif

using namespace erpc;

int sockRPMsgRTOSTransport::sock_fd = -1;
uint16_t sockRPMsgRTOSTransport::remote_port;
bool sockRPMsgRTOSTransport::server_role = false;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

sockRPMsgRTOSTransport::sockRPMsgRTOSTransport()
: Transport()
{
}

sockRPMsgRTOSTransport::~sockRPMsgRTOSTransport()
{
    rpmsg_socket_close(sock_fd);
}

erpc_status_t sockRPMsgRTOSTransport::init(uint16_t port, bool serverRole)
{
    int ret_value, new_fd;
    sockaddr_rpmsg sockaddr;

    sockaddr.family = AF_RPMSG;
    sockaddr.vproc_id = 0;

    new_fd = rpmsg_socket_create(RPMSG_ST_DGRAM);
    if(new_fd < 0){
        return kErpcStatus_InitFailed;
    }

    server_role = serverRole;

    if(serverRole){
        sockaddr.addr = port;
        ret_value = rpmsg_socket_bind(new_fd, &sockaddr);
        if(ret_value < 0){
            rpmsg_socket_close(new_fd);
            fl_printf("CPU1: ERROR binding socket %d: %d\r\n", sockaddr.addr, ret_value);
            return kErpcStatus_InitFailed;
        }
    } else {
        sockaddr.addr = 0;
        ret_value = rpmsg_socket_bind(new_fd, &sockaddr);
        if(ret_value < 0){
            rpmsg_socket_close(new_fd);
            fl_printf("CPU1: ERROR binding socket %d: %d\r\n", sockaddr.addr, ret_value);
            return kErpcStatus_InitFailed;
        }

        remote_port = port;
    }

    fl_printf("new socket created: %d:%d", new_fd, sockaddr.addr);

    sock_fd = new_fd;

    return kErpcStatus_Success;
}

erpc_status_t sockRPMsgRTOSTransport::receive(MessageBuffer *message)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    sockaddr_rpmsg remote_addr;

    uint32_t length = message->getLength();
    uint8_t *freeBuffer = message->get();


    sockaddr_rpmsg local_addr;
    rpmsg_socket_getsockname(sock_fd, &local_addr);
    fl_printf("waiting for message on socket %d:%d", sock_fd, local_addr.addr);


    ret_value = rpmsg_socket_recvfrom(sock_fd, freeBuffer, length, &remote_addr);
    if(ret_value < 0){
        fl_printf("CPU1: ERROR receiving message: %d\r\n", ret_value);
        return kErpcStatus_ReceiveFailed;
    }

    fl_printf("received message %s from %d:%d", freeBuffer, remote_addr.vproc_id, remote_addr.addr);

    if(server_role){
    //XXX: this assumes only one remote can be contacted at a time
        remote_port = remote_addr.addr;
    }

    return kErpcStatus_Success;
}

erpc_status_t sockRPMsgRTOSTransport::send(MessageBuffer *message)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    sockaddr_rpmsg remote_addr;

    remote_addr.addr = remote_port;
    remote_addr.family = AF_RPMSG;
    remote_addr.vproc_id = 0;

    fl_printf("sending message %s to %d:%d", message->get(), remote_addr.vproc_id, remote_addr.addr);

    ret_value = rpmsg_socket_sendto(sock_fd, (void *)message->get(), message->getUsed(), &remote_addr);
    if(ret_value < 0){
        fl_printf("CPU1: ERROR sending message: %d\r\n", ret_value);
        return kErpcStatus_SendFailed;
    }

    fl_printf("message sent");

    return kErpcStatus_Success;
}
