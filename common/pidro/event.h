#ifndef PIDRO_EVENT_H
#define PIDRO_EVENT_H

#include <iostream>

namespace Pidro
{
  
enum class Event
{
    GAME_INIT = 0,
    ROUND_INIT,
    DEAL_CARD,
    ASK_FOR_BID,
    BID_PLACED,
    GOT_BID,
    COLOR_SELECTED,
    PUT_CARDS_ON_TABLE,
    KILL_CARD,
    BEGIN_PLAY,
    PLAY_CARD,
    PLAY_ROUND_FINISH,
    PLAYER_COLD,
    ROUND_OVER,
    GAME_OVER
};

const char* toString(Event ev);

inline std::ostream& operator << (std::ostream& out, Event ev)
{
    out << toString(ev);
    return out;
}

}

#endif // EVENT_H
