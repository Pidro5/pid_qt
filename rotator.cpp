#include "rotator.h"

Rotator::Rotator()
{
   my_position = 0;
   my_start_position= 0;
}

Rotator::Rotator(int pos)
{
    my_position = pos;
    my_start_position = pos;
}

Rotator::~Rotator()
{

}

void Rotator::set_position(int pos)
{
    if (pos >= 0) {
        my_position = (pos % 4);
        my_start_position = my_position;
    }
    else{
        my_position = 0;
        my_start_position = 0;
    }
}

int Rotator::get_position() const
{
    return my_position;
}

int Rotator::get_my_start_position() const
{
    return my_start_position;
}

Rotator& Rotator::operator ++(){
    my_position = (my_position + 1) % 4;
    return *this;
}

Rotator Rotator::operator ++(int ){
    Rotator temp(*this);

    my_position = (my_position + 1) % 4;
    return temp;
}

Rotator& Rotator::operator --(){
    my_position--;
    if (my_position < 0) {
        my_position =3;
    }
    return *this;
}

Rotator Rotator::operator --(int ){
    Rotator temp(*this);

    my_position--;
    if (my_position < 0) {
        my_position =3;

    }
    return temp;

}
