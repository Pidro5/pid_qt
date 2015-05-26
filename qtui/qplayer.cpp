#include "qplayer.h"
#include <iostream>

using namespace std;

QPlayer::QPlayer(const std::string& name, QObject* pParent)
    : QObject(pParent)
    , Player(name)
{
}

QPlayer::~QPlayer()
{

}

bool QPlayer::inform_event(Event et)
{
    cout << et << endl;
    return true;
}

bool QPlayer::inform_event(Event et, int position)
{
    cout << et << " " << position << endl;
    return true;
}

bool QPlayer::inform_event(Event et, int position, int value)
{
    cout << et << " " << position << " " << value << endl;
    return true;
}

bool QPlayer::inform_event(Event et, int position, std::list<Card *>& cards)
{
    cout << et << " " << position << " " << "..." << endl;
    return true;
}
