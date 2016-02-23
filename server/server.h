/*! \file server.h

    Makes new server
 */
#pragma once
#include "requestparsingstate.h"
#include "clientstate.h"
#include "response.h"

namespace server
{
    
class Server
{
public:
    /*! Parses request
        \param[in] a state
        \return whether request was parsed
     */
    static server::RequestParsingState* parseRequest(server::ClientState* state);
    /*! Converts state to response
        \param[in] state a state response
        \return response
     */
    static server::Response* stateToResponse(server::RequestParsingState* state);
};

}