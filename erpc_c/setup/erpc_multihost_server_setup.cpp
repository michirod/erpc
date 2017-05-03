/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
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

#include "erpc_multihost_server_setup.h"
#include "basic_codec.h"
#include "manually_constructed.h"
#include "multihost_server.h"
#include <assert.h>
#include <new>

#if !(__embedded_cplusplus)
using namespace std;
#endif

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// global server variables
static ManuallyConstructed<MultihostServer> s_multihost_server;
MultihostServer *g_multihost_server;

static ManuallyConstructed<BasicCodecFactory> s_multihost_codecFactory;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void erpc_multihost_server_init(erpc_transport_t transport, erpc_transport_t portmapper_transport, erpc_mbf_t message_buffer_factory)
{
    // Init factories.
    s_multihost_codecFactory.construct();

    // Init server with the provided transport.
    s_multihost_server.construct();
    s_multihost_server->setTransport(reinterpret_cast<MultihostTransport *>(transport));
    s_multihost_server->setPortmapperTransport(reinterpret_cast<Transport *>(portmapper_transport));
    s_multihost_server->setCodecFactory(s_multihost_codecFactory);
    s_multihost_server->setMessageBufferFactory(reinterpret_cast<MessageBufferFactory *>(message_buffer_factory));
    g_multihost_server = s_multihost_server;
}

void erpc_multihost_server_deinit()
{
    s_multihost_codecFactory.destroy();
    s_multihost_server.destroy();
}

void erpc_add_service_to_multihost_server(void *service)
{
    if (service != NULL)
    {
        g_multihost_server->addService(static_cast<erpc::Service *>(service));
    }
}

erpc_status_t erpc_multihost_server_run()
{
    return g_multihost_server->run();
}

erpc_status_t erpc_multihost_server_poll()
{
    return g_multihost_server->poll();
}

void erpc_multihost_server_stop()
{
    g_multihost_server->stop();
}
