#include "clientstate.h"

server::ClientState::ClientState() : Responded(false)
{
    
}

server::ClientState::~ClientState()
{
    
}

void server::ClientState::append(char* buf, int length)
{
    for(int i = 0; i < length; i++)
    {
        Request.push_back(buf[i]);
    }
}