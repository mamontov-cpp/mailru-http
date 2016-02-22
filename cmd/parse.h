/*! \file parse.h

    Contains a definition for parsing a command line
 */
#include <string>

namespace cmd
{

/*! Options for server configuration
 */
struct Options
{
    /*! IP address
     */
    std::string IP;
    /*! A port for server
     */
    size_t Port;
    /*! A directory, where files, are located
     */
    std::string Directory;

    /*! A default options
     */
    inline Options() : IP("127.0.0.1"), Port(80), Directory(".")
    {
        
    }
};

/*! Parses options
    \param[in] argc an argument count
    \param[in] argv an arguments
    \param[out] opts options
 */
void parse(int argc, char** argv, cmd::Options& opts);

}
