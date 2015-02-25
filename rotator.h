#ifndef ROTATOR_H
#define ROTATOR_H


class Rotator
{
    int my_position;
    int my_start_position;

public:
    Rotator();
    Rotator(int pos);
    ~Rotator();

    void set_position(int pos);
    int get_position() const;
    int get_my_start_position() const;


    Rotator& operator++ ();   // prefix
    Rotator operator++ (int); // postfix

    Rotator& operator-- ();    // prefix
    Rotator operator-- (int);  // postfix

};

#endif // ROTATOR_H
