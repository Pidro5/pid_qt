#include "qpidroresult.h"

QPidroResult::QPidroResult()
    : m_sem(0)
{
}

QPidroResult::~QPidroResult()
{
}

void QPidroResult::wait()
{
    m_sem.acquire(1);
}

void QPidroResult::post()
{
    m_sem.release(1);
}

