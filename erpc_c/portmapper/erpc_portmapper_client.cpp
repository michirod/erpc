/*
 * Generated by erpcgen 1.4.0 on Tue May  2 12:52:34 2017.
 *
 * AUTOGENERATED - DO NOT EDIT
 */

#include "erpc_portmapper.h"
#include "client_manager.h"
#include "transport.h"
#include "erpc_port.h"

using namespace erpc;
#if !(__embedded_cplusplus)
using namespace std;
#endif

extern ClientManager *g_client;

// Constant variable definitions
const uint16_t portmapper_port = 111;

//! @brief Function to write struct pm_entry
static int32_t write_pm_entry_struct(erpc::Codec * codec, const pm_entry * data);

// Write struct pm_entry function implementation
static int32_t write_pm_entry_struct(erpc::Codec * codec, const pm_entry * data)
{
    erpc_status_t err = codec->startWriteStruct();
    if (!err)
    {
        err = codec->write(data->program_id);
    }

    if (!err)
    {
        err = codec->write(static_cast<int32_t>(data->protocol));
    }

    if (!err)
    {
        err = codec->write(data->core_id);
    }

    if (!err)
    {
        err = codec->write(data->port);
    }

    if (!err)
    {
        err = codec->endWriteStruct();
    }
    return err;
}

//! @brief Function to read struct pm_entry
static int32_t read_pm_entry_struct(erpc::Codec * codec, pm_entry * data);

//! @brief Function to read struct list_0_t
static int32_t read_list_0_t_struct(erpc::Codec * codec, list_0_t * data);

// Read struct pm_entry function implementation
static int32_t read_pm_entry_struct(erpc::Codec * codec, pm_entry * data)
{
    int32_t enum_tmp_local;
    erpc_status_t err = codec->startReadStruct();
    if (!err)
    {
        err = codec->read(&data->program_id);
    }

    if (!err)
    {
        err = codec->read(&enum_tmp_local);
        if (!err)
        {
            data->protocol = static_cast<pm_protocols>(enum_tmp_local);
        }
    }

    if (!err)
    {
        err = codec->read(&data->core_id);
    }

    if (!err)
    {
        err = codec->read(&data->port);
    }

    if (!err)
    {
        err = codec->endReadStruct();
    }
    return err;
}

// Read struct list_0_t function implementation
static int32_t read_list_0_t_struct(erpc::Codec * codec, list_0_t * data)
{
    erpc_status_t err = codec->startReadStruct();
    if (!err)
    {
        err = codec->startReadList(&data->elementsCount);
        if (!err)
        {
            if (data->elementsCount > 0)
            {
                data->elements = (pm_entry *) erpc_malloc(data->elementsCount * sizeof(pm_entry));
                if (data->elements == NULL)
                {
                    err = kErpcStatus_MemoryError;
                }
                else
                {
                        for (uint32_t listCount = 0; listCount < data->elementsCount; ++listCount)
                        {
                            if (!err)
                            {
                                err = read_pm_entry_struct(codec, &(data->elements[listCount]));
                            }
                            else
                            {
                                break;
                            }
                        }
                }
            }
            else
            {
                data->elements = NULL;
            }
        }
        if (!err)
        {
            err = codec->endReadList();
        }
    }

    if (!err)
    {
        err = codec->endReadStruct();
    }
    return err;
}


// portmapper interface register_service function client shim.
bool register_service(uint32_t program_id, pm_protocols protocol, int16_t core_id, uint16_t port)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_register_service_id, request.getSequence());
    }
    if (!err)
    {
        err = codec->write(program_id);
    }

    if (!err)
    {
        err = codec->write(static_cast<int32_t>(protocol));
    }

    if (!err)
    {
        err = codec->write(core_id);
    }

    if (!err)
    {
        err = codec->write(port);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    bool result;
    if (!err)
    {
        err = codec->read(&result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return false;
    }

    return result;
}

// portmapper interface register_entry function client shim.
bool register_entry(const pm_entry * entry)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_register_entry_id, request.getSequence());
    }
    if (!err)
    {
        err = write_pm_entry_struct(codec, entry);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    bool result;
    if (!err)
    {
        err = codec->read(&result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return false;
    }

    return result;
}

// portmapper interface unregister_service function client shim.
bool unregister_service(uint32_t program_id, pm_protocols protocol, int16_t core_id, uint16_t port)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_unregister_service_id, request.getSequence());
    }
    if (!err)
    {
        err = codec->write(program_id);
    }

    if (!err)
    {
        err = codec->write(static_cast<int32_t>(protocol));
    }

    if (!err)
    {
        err = codec->write(core_id);
    }

    if (!err)
    {
        err = codec->write(port);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    bool result;
    if (!err)
    {
        err = codec->read(&result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return false;
    }

    return result;
}

// portmapper interface unregister_entry function client shim.
bool unregister_entry(const pm_entry * entry)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_unregister_entry_id, request.getSequence());
    }
    if (!err)
    {
        err = write_pm_entry_struct(codec, entry);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    bool result;
    if (!err)
    {
        err = codec->read(&result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return false;
    }

    return result;
}

// portmapper interface lookup function client shim.
pm_entry_list * lookup(uint32_t program_id)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_lookup_id, request.getSequence());
    }
    if (!err)
    {
        err = codec->write(program_id);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    pm_entry_list * result;
    result = (pm_entry_list *) erpc_malloc(sizeof(pm_entry_list));
    if (result == NULL)
    {
        err = kErpcStatus_MemoryError;
    }
    if (!err)
    {
        err = read_list_0_t_struct(codec, result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return NULL;
    }

    return result;
}

// portmapper interface lookup_proto function client shim.
pm_entry_list * lookup_proto(uint32_t program_id, pm_protocols protocol)
{
    erpc_status_t err = kErpcStatus_Success;

    // Get a new request.
    RequestContext request = g_client->createRequest(false);

    // Encode the request.
    Codec * codec = request.getCodec();
    if (codec == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    if (!err)
    {
        err = codec->startWriteMessage(kInvocationMessage, kportmapper_service_id, kportmapper_lookup_proto_id, request.getSequence());
    }
    if (!err)
    {
        err = codec->write(program_id);
    }

    if (!err)
    {
        err = codec->write(static_cast<int32_t>(protocol));
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    // Send message to server
    if (!err)
    {
        err = g_client->performRequest(request);
    }

    pm_entry_list * result;
    result = (pm_entry_list *) erpc_malloc(sizeof(pm_entry_list));
    if (result == NULL)
    {
        err = kErpcStatus_MemoryError;
    }
    if (!err)
    {
        err = read_list_0_t_struct(codec, result);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Dispose of the request.
    g_client->releaseRequest(request);

    if (err)
    {
        client_error_handler_t errorHandler = g_client->getErrorHandler();
        if (errorHandler != NULL)
        {
            (*errorHandler)(err);
        }
        return NULL;
    }

    return result;
}
