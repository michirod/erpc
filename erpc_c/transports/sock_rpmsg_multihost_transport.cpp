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

#include "sock_rpmsg_multihost_transport.h"
#include "errno.h"
#include <new>
#include "stdio.h"

#if !(__embedded_cplusplus)
using namespace std;
#endif

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

sockRPMsgMultihostTransport::sockRPMsgMultihostTransport()
: MultihostTransport()
{
}

sockRPMsgMultihostTransport::~sockRPMsgMultihostTransport()
{
    close(sock_fd);
}


erpc_status_t sockRPMsgMultihostTransport::init(uint16_t port, uint16_t remote_vproc_id, bool serverRole)
{
    int ret_value, new_fd;
    struct sockaddr_rpmsg sockaddr;

    sockaddr.family = AF_RPMSG;
    sockaddr.vproc_id = remote_vproc_id;

    new_fd = socket(AF_RPMSG, SOCK_DGRAM, 0);
    if(new_fd < 0){
        perror("Error");
        return kErpcStatus_InitFailed;
    }

    if(serverRole){
        sockaddr.addr = port;
        ret_value = bind(new_fd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
        if(ret_value < 0){
            perror("Error");
            return kErpcStatus_InitFailed;
        }
    } else {
        return kErpcStatus_InvalidArgument;
    }

    sock_fd = new_fd;

    return kErpcStatus_Success;
}



erpc_status_t sockRPMsgMultihostTransport::select(MessageBuffer *message, int *client_id)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    struct sockaddr_rpmsg remote_addr;

    uint32_t length = message->getLength();
    uint8_t *freeBuffer = message->get();
    socklen_t addr_len = sizeof(struct sockaddr_rpmsg);

    ret_value = recvfrom(sock_fd, freeBuffer, length, 0,
                         (struct sockaddr *) &remote_addr, &addr_len);
    if(ret_value < 0){
        perror("Error");
        return kErpcStatus_ReceiveFailed;
    }

    (*client_id) = (remote_addr.vproc_id & 0xFFFF) | (remote_addr.addr & 0xFFFF) << 16;

    printf("received message %s from %d:%d, client id: %d (%x)\n", freeBuffer,
           remote_addr.vproc_id, remote_addr.addr, *client_id, *client_id);

    return kErpcStatus_Success;
}

erpc_status_t sockRPMsgMultihostTransport::send(const MessageBuffer *message, int client_id)
{
    while (sock_fd < 0)
    {
    }

    int ret_value;
    sockaddr_rpmsg remote_addr;

    remote_addr.addr = (client_id >> 16) & 0xFFFF;
    remote_addr.family = AF_RPMSG;
    remote_addr.vproc_id = client_id & 0xFFFF;

    printf("sending message %s to %d:%d, client id: %d (%x)\n", message->get(),
           remote_addr.vproc_id, remote_addr.addr, client_id, client_id);

    ret_value = sendto(sock_fd, (void *)message->get(), message->getUsed(), 0,
                       (struct sockaddr *)&remote_addr, sizeof(remote_addr));
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
    printf("message sent\n");
    return kErpcStatus_Success;
}
