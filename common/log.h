#ifndef LOG_H
#define LOG_H

#include <string>
#include <cstdlib>

namespace Log
{
    enum Entry
    {
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    const char* toString(Entry entry);

    std::ostream& stream();
    void setStream(std::ostream* pStream);
}

#define LOG(kind, expression)\
    do {\
        Log::stream() << Log::toString(kind) << __FILE__ << "(" << __LINE__ << "): " << expression << std::endl;\
        if (kind == Log::Entry::FATAL) {\
            abort();\
        }\
    } while (false)

#define LOG_NO_FILE_LINE(kind, expression)\
    do {\
        Log::stream() << expression << std::endl;\
        if (kind == Log::Entry::FATAL) {\
            abort();\
        }\
    } while (false)



#define LOG_D(expression) LOG(Log::DEBUG, expression)
#define LOG_W(expression) LOG(Log::WARNING, expression)
#define LOG_E(expression) LOG(Log::ERROR, expression)
#define LOG_F(expression) LOG(Log::FATAL, expression)

#define LOG_BIDRULE(expression) LOG_NO_FILE_LINE(Log::DEBUG, expression)

#endif // LOG_H
