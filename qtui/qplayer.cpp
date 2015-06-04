#include "qplayer.h"
#include <cassert>
#include <iostream>
#include <QtQml>
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

//static
void QPlayer::declareQML()
{
    qmlRegisterUncreatableType<QPlayer>("pidro.components", 1, 0, "Player",
                                        QString("Player cannot be instantiated from QML"));
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
    pEvent->deliverTo(*this);

    return true;
}

bool QPlayer::inform_event(Pidro::Event et)
{
    LOG_D(et);

    switch (et) {
    case Pidro::Event::GAME_INIT:
        break;

    case Pidro::Event::BEGIN_PLAY:
        break;

    case Pidro::Event::PLAY_ROUND_FINISH:
        break;

    case Pidro::Event::ROUND_OVER:
        break;

    case Pidro::Event::GAME_OVER:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool QPlayer::inform_event(Pidro::Event ev, int position)
{
    LOG_D(ev << position);

    switch (ev) {
    case Pidro::Event::ROUND_INIT:
        break;

    case Pidro::Event::ASK_FOR_BID:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool QPlayer::inform_event(Pidro::Event ev, int position, int value)
{
    LOG_D(ev << " " << position << " " << value);

    switch (ev) {
    case Pidro::Event::DEAL_CARD:
        break;

    case Pidro::Event::BID_PLACED:
        break;

    case Pidro::Event::GOT_BID:
        break;

    case Pidro::Event::COLOR_SELECTED:
        break;

    default:
        assert(!true);
    }

    return true;
}

bool QPlayer::inform_event(Pidro::Event ev, int position, std::list<Pidro::Card*>& /*cards*/)
{
    LOG_D(ev << " " << position << " " << "...");

    switch (ev) {
    case Pidro::Event::DEAL_CARD:
        break;

    case Pidro::Event::PUT_CARDS_ON_TABLE:
        break;

    case Pidro::Event::KILL_CARD:
        break;

    case Pidro::Event::PLAY_CARD:
        break;

    case Pidro::Event::PLAYER_COLD:
        break;

    default:
        assert(!true);
    }

    return true;
}

int QPlayer::give_bid(int minimum)
{
    LOG_D("give_bid");
    return 0;
}

int QPlayer::give_color()
{
    LOG_D("give_color");
    return 0;
}

Pidro::Card* QPlayer::play_card(int color)
{
    LOG_D("play_card");
    return 0;
}
