#include "qpidroresult.h"

QPidroResult::QPidroResult(bool code)
    : m_sem(0)
    , m_code(code)
{

}

QPidroResult::~QPidroResult()
{

}

void QPidroResult::wait()
{
    m_sem.acquire(1);
}

void QPidroResult::setCode(bool code)
{
    m_code = code;
    m_sem.release(1);
}

bool QPidroResult::code() const
{
    return m_code;
}

