#ifndef PIDRO_LOG_H
#define PIDRO_LOG_H

#include <string>
#include <cstdlib>

namespace Pidro
{
  
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

}

#define LOG(kind, expression)\
    do {\
      Pidro::Log::stream() << Pidro::Log::toString(kind) << __FILE__ << "(" << __LINE__ << "): " << expression << std::endl; \
      if (kind == Pidro::Log::Entry::FATAL) {\
        abort();\
      }\
    } while (false)



#define LOG_D(expression) LOG(Pidro::Log::DEBUG, expression)
#define LOG_W(expression) LOG(Pidro::Log::WARNING, expression)
#define LOG_E(expression) LOG(Pidro::Log::ERROR, expression)
#define LOG_F(expression) LOG(Pidro::Log::FATAL, expression)

#endif // LOG_H
