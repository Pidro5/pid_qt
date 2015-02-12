#include "log.h"
#include <iostream>


using namespace std;

void Log::println()
{
    cout << endl;
}


void Log::print(string str )
{
    cout << str ;
}

void Log::println(string str )
{
    cout << str << endl;
}

void Log::print(int i )
{

    char buf[10];

    sprintf(buf,"%d",i);
    cout << buf ;
}

void Log::println(int i )
{
    char buf[10];

    sprintf(buf,"%d",i);
    cout << buf << endl;
}


void Log::print(ostream str )
{
    cout << str ;
}

void Log::println(ostream str )
{
    cout << str << endl;
}
