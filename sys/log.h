/*! \file log.h

    A log file
 */
#pragma once
#include "mutex.h"
#include <cstdio>
#include <stdarg.h>

namespace sys
{
    
class Log
{
public:    
    /*! A log function
     */
    Log();
    /*! Closes file, attached to log
     */
    ~Log();
    /*! Writes data to a log
        \param[in] format a format
     */
    static void write(const char* format, ...);
    /*! Tries to open new log
        \return whether it was successfull
     */
    static bool open();
private:
    /*! A file to be written
     */
    FILE* m_file;
    /*! A mutex file 
     */
    sys::Mutex m_mtx;

};

}