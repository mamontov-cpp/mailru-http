/*! \file requestparsingstate.h
    
    Describes a state of request parsing
 */
#include <string>

namespace server
{
/*! Describes a state for parsing request
 */ 
struct RequestParsingState
{
    std::string URI; //!< An URI
    std::string BaseURI; //!< A base URI
    bool Valid; //!< Whether request is valid
    bool Complete; //!< Whether request is complete

    inline RequestParsingState() : Valid(false), Complete(false)
    {
        
    }
};

}
