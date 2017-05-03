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

#include "multihost_server.h"
#include "erpc_portmapper.h"
#include "erpc_client_setup.h"

using namespace erpc;
#if !(__embedded_cplusplus)
using namespace std;
#endif

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

MultihostServer::~MultihostServer()
{
    while (m_firstService != NULL)
    {
        Service *firstService = m_firstService;
        m_firstService = m_firstService->getNext();
        if(m_pm_transport){
            unregister_service(reinterpret_cast<erpc_transport_t>(m_pm_transport),
                             firstService->getServiceId(), (pm_protocols) m_transport->getProtocol(),
                             m_transport->getAddr());
        }
        delete firstService;
    }
}

void MultihostServer::addService(Service *service){
    Server::addService(service);

    //register service to portmapper
    if(m_pm_transport){
        erpc_client_init(reinterpret_cast<erpc_transport_t>(m_pm_transport),
                         reinterpret_cast<erpc_mbf_t>(m_messageFactory));
        register_service(reinterpret_cast<erpc_transport_t>(m_pm_transport),
                         service->getServiceId(), (pm_protocols) m_transport->getProtocol(),
                         m_transport->getAddr());
    }
}


void MultihostServer::disposeBufferAndCodec(Codec *codec)
{
    if (codec)
    {
        if (codec->getBuffer())
        {
            m_messageFactory->dispose(codec->getBuffer());
        }
        m_codecFactory->dispose(codec);
    }
}

Codec *MultihostServer::createBufferAndCodec()
{
    Codec *codec = m_codecFactory->create();
    if (!codec)
    {
        return NULL;
    }

    MessageBuffer message = m_messageFactory->create();
    if (!message.get())
    {
        // Dispose of buffers and codecs.
        disposeBufferAndCodec(codec);
        return NULL;
    }

    codec->setBuffer(message);

    return codec;
}

erpc_status_t MultihostServer::runInternal()
{
    // Create codec to read the request.
    Codec *codec = createBufferAndCodec();
    if (!codec)
    {
        return kErpcStatus_MemoryError;
    }

    int client_id;

    // Receive the next invocation request.
    erpc_status_t err = m_transport->select(codec->getBuffer(), &client_id);
    if (err)
    {
        // Dispose of buffers and codecs.
        disposeBufferAndCodec(codec);
        return err;
    }

    // If is used RPMsg transport layer, the receive function changes
    // pointer to buffer(m_buf) of requestMessage, inCodec must be reseted
    // after this change to work with correct buffer
    codec->reset();

    // Handle the request.
    message_type_t msgType;
    int remote_core = CLIENT_ID_TO_CORE_ID(client_id);
    err = processMessage(codec, msgType, remote_core);
    if (err)
    {
        // Dispose of buffers and codecs.
        disposeBufferAndCodec(codec);
        return err;
    }

    if (msgType != kOnewayMessage)
    {
        err = m_transport->send(codec->getBuffer(), client_id);
    }

    // Dispose of buffers and codecs.
    disposeBufferAndCodec(codec);

    return err;
}

erpc_status_t MultihostServer::run()
{
    erpc_status_t err = kErpcStatus_Success;
    while (!err && m_isServerOn)
    {
        err = runInternal();
    }
    return err;
}

erpc_status_t MultihostServer::poll()
{
    if (m_isServerOn)
    {
        if (m_transport->hasMessage())
        {
            return runInternal();
        }
        else
        {
            return kErpcStatus_Success;
        }
    }
    return kErpcStatus_ServerIsDown;
}

void MultihostServer::stop()
{
    m_isServerOn = false;
}
