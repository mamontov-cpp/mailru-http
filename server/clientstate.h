/*! \file clientstate.h
    
    Describes a simple client state for a client, connected, through socket
 */
#pragma once
#include <vector>

namespace server
{

struct ClientState
{
public:
    /*! A requested client state
     */
    std::vector<char> Request;
    /*! Whether we are already responded to
     */
    bool Responded;
    /*! Makes new state
     */
    ClientState();
    /*! Destroys state
     */
    ~ClientState();
    /*! Appends buffer to state
        \param[in] buf buffer
        \param[in] length a length
     */
    void append(char* buf, int length);
};

}
