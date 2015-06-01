#include "qplayer.h"
#include <cassert>
#include <iostream>
#include "qpidroevent.h"
#include "qpidroresult.h"

using namespace std;

QPlayer::QPlayer(QObject* pParent)
    : QObject(pParent)
{
}

QPlayer::~QPlayer()
{
}

bool QPlayer::event(QEvent *pEvent)
{
    // TODO: Figure out who owns pEvent.

    bool rv = false;

    QPidroEvent* pPidroEvent = dynamic_cast<QPidroEvent*>(pEvent);

    if (pPidroEvent) {
        rv = event(pPidroEvent);
    }

    return rv;
}

bool QPlayer::event(QPidroEvent *pEvent)
{
    pEvent->result()->setCode(pEvent->deliverTo(*this));

    return true;
}

bool QPlayer::inform_event(Event et)
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

bool QPlayer::inform_event(Event ev, int position)
{
    LOG_D(ev << position);

    switch (ev) {
    case Event::ROUND_INIT:
        break;

    case Event::ASK_FOR_BID:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool QPlayer::inform_event(Event ev, int position, int value)
{
    LOG_D(ev << " " << position << " " << value);

    switch (ev) {
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

bool QPlayer::inform_event(Event ev, int position, std::list<Card *>& /*cards*/)
{
    LOG_D(ev << " " << position << " " << "...");

    switch (ev) {
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
