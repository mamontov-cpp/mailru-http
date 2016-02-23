/*! \file response.h
    
    Describes a sever response
 */
#include <vector>
// ReSharper disable once CppUnusedIncludeDirective
#include <string>

namespace server
{

class Response
{
public:
    /*! A new response
     */
    Response();
    /*! Returns body
        \return body
     */
    char* body() const;
    /*! A length of response
        \return length of response
     */
    int length() const;
    /*! Returns code for resposne
        \return code for response
     */
    int code() const;
    /*! Sets body for response
        \param[in] body a body
     */
    void setBody(const std::string& body);
    /*! Sets code for response
        \param[in] code a code
     */ 
    void setCode(int code);   
    /*! Makes a response
        \param[in] code a status code
        \param[in] reason a reason
        \param[in] body a body
        \return response
     */
    static server::Response* makeResponse(int code, const std::string& reason, const std::string& body);
protected:
    /*! A code for response
     */
    int m_code;
    /*! A body for response
     */
    std::string m_body;
};

/*! Responds 400 "Bad Request"
    \return 400 "Bad Request"
 */
server::Response* respond400();

/*! Responds 404 "Not Found"
    \return 404 "Not Found"
 */
server::Response* respond404();

/*! Responds 200 "OK"
    \return 200 "OK"
 */
server::Response* respond200(const std::string& body);

}

