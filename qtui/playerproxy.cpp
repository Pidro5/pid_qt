#include "playerproxy.h"
#include <cassert>
#include <iostream>
#include <QCoreApplication>
#include "qplayer.h"
#include "qpidroevent.h"
#include "qpidroresult.h"

using namespace std;

PlayerProxy::PlayerProxy(const std::string& name, QPlayer* pPlayer)
  : Player(name)
  , m_pPlayer(pPlayer)
{
}

PlayerProxy::~PlayerProxy()
{
}

bool PlayerProxy::inform_event(Pidro::Event ev)
{
    LOG_D(ev);
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return postAndWait(new QPidroInfoEvent1(sResult, ev));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position)
{
    LOG_D(ev << ", " << position);
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return postAndWait(new QPidroInfoEvent2(sResult, ev, position));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position, int value)
{
    LOG_D(ev << ", " << position << ", " << value);
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return postAndWait(new QPidroInfoEvent3(sResult, ev, position, value));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position, std::list<Pidro::Card*>& cards)
{
    LOG_D(ev << ", " << position << ", " << "...");
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return postAndWait(new QPidroInfoEvent4(sResult, ev, position, cards));
}

int PlayerProxy::give_bid(int minimum)
{
    LOG_D("giveBid: " << minimum);
    shared_ptr<QPidroResultInt> sResult(new QPidroResultInt);

    return postAndWait(new QPidroCommandGiveBid(sResult, minimum));
}

Pidro::Card::Suit PlayerProxy::give_suit()
{
    LOG_D("giveSuit");
    shared_ptr<QPidroResultInt> sResult(new QPidroResultInt);

    return static_cast<Pidro::Card::Suit>(postAndWait(new QPidroCommandGiveColor(sResult)));
}

Pidro::Card* PlayerProxy::play_card(Pidro::Card::Suit suit)
{
    LOG_D("playCard:" << suit);
    shared_ptr<QPidroResultCard> sResult(new QPidroResultCard);

    /*return*/ postAndWait(new QPidroCommandPlayCard(sResult, suit));
    return Player::play_card(suit);
}

bool PlayerProxy::postAndWait(QPidroEventT<bool>* pEvent)
{
    shared_ptr<QPidroResultBool> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}

int PlayerProxy::postAndWait(QPidroEventT<int>* pEvent)
{
    shared_ptr<QPidroResultInt> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}

Pidro::Card* PlayerProxy::postAndWait(QPidroEventT<Pidro::Card*>* pEvent)
{
    shared_ptr<QPidroResultCard> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}
