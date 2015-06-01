#include "log.h"
#include <cassert>
#include <iostream>

using namespace std;

namespace
{

ostream* u_pLogStream = &cout;

}

namespace Pidro
{

ostream& Log::stream()
{
    assert(u_pLogStream);

    return *u_pLogStream;
};

void Log::setStream(std::ostream *pLogStream)
{
    assert(pLogStream);

    u_pLogStream = pLogStream;
}

const char* Log::toString(Log::Entry entry)
{
    switch (entry) {
    case Log::DEBUG:
        return "D";

    case Log::WARNING:
        return "W";

    case Log::ERROR:
        return "E";

    case Log::FATAL:
        return "F";
    }

    assert(!true);
    return "U"; // As in unknown.
}

}
