#include "playerproxy.h"
#include <cassert>
#include <iostream>

using namespace std;

PlayerProxy::PlayerProxy(const std::string& name)
  : Player(name)
{
}

PlayerProxy::~PlayerProxy()
{
}

bool PlayerProxy::inform_event(Event et)
{
    LOG_D(et);

    switch (et) {
    case Event::GAME_INIT:
        break;

    case Event::BEGIN_PLAY:
        break;

    case Event::PLAY_ROUND_FINISH:
        break;

    case Event::ROUND_OVER:
        break;

    case Event::GAME_OVER:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool PlayerProxy::inform_event(Event et, int position)
{
    LOG_D(et << " " << position);

    switch (et) {
    case Event::ROUND_INIT:
        break;

    case Event::ASK_FOR_BID:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool PlayerProxy::inform_event(Event et, int position, int value)
{
    LOG_D(et << " " << position << " " << value);

    switch (et) {
    case Event::DEAL_CARD:
        break;

    case Event::BID_PLACED:
        break;

    case Event::GOT_BID:
        break;

    case Event::COLOR_SELECTED:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool PlayerProxy::inform_event(Event et, int position, std::list<Card *>& /*cards*/)
{
    LOG_D(et << " " << position << " " << "...");

    switch (et) {
    case Event::DEAL_CARD:
        break;

    case Event::PUT_CARDS_ON_TABLE:
        break;

    case Event::KILL_CARD:
        break;

    case Event::PLAY_CARD:
        break;

    case Event::PLAYER_COLD:
        break;

    default:
        assert(!true);
    }

    return true;
}
