#include "rotator.h"

Rotator::Rotator()
{
   m_my_position = 0;
   m_my_start_position= 0;
}

Rotator::Rotator(int pos)
{
    m_my_position = pos;
    m_my_start_position = pos;
}

Rotator::~Rotator()
{

}

void Rotator::set_position(int pos)
{
    if (pos >= 0) {
        m_my_position = (pos % 4);
        m_my_start_position = m_my_position;
    }
    else{
        m_my_position = 0;
        m_my_start_position = 0;
    }
}

int Rotator::get_position() const
{
    return m_my_position;
}

int Rotator::get_my_start_position() const
{
    return m_my_start_position;
}

Rotator& Rotator::operator ++(){
    m_my_position = (m_my_position + 1) % 4;
    return *this;
}

Rotator Rotator::operator ++(int ){
    Rotator temp(*this);

    m_my_position = (m_my_position + 1) % 4;
    return temp;
}

Rotator& Rotator::operator --(){
    m_my_position--;
    if (m_my_position < 0) {
        m_my_position =3;
    }
    return *this;
}

Rotator Rotator::operator --(int ){
    Rotator temp(*this);

    m_my_position--;
    if (m_my_position < 0) {
        m_my_position =3;

    }
    return temp;

}
