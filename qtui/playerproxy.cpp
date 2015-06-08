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
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return deliverEvent(new QPidroInfoEvent1(sResult, ev));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position)
{
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return deliverEvent(new QPidroInfoEvent2(sResult, ev, position));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position, int value)
{
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return deliverEvent(new QPidroInfoEvent3(sResult, ev, position, value));
}

bool PlayerProxy::inform_event(Pidro::Event ev, int position, std::list<Pidro::Card*>& cards)
{
    shared_ptr<QPidroResultBool> sResult(new QPidroResultBool);

    return deliverEvent(new QPidroInfoEvent4(sResult, ev, position, cards));
}

int PlayerProxy::give_bid(int minimum)
{
    shared_ptr<QPidroResultInt> sResult(new QPidroResultInt);

    /*return*/ deliverEvent(new QPidroCommandGiveBid(sResult, minimum));
    return Player::give_bid(minimum);
}

Pidro::Card::Suit PlayerProxy::give_suit()
{
    shared_ptr<QPidroResultInt> sResult(new QPidroResultInt);

    /*return*/ deliverEvent(new QPidroCommandGiveColor(sResult));
    return Player::give_suit();
}

Pidro::Card* PlayerProxy::play_card(Pidro::Card::Suit suit)
{
    shared_ptr<QPidroResultCard> sResult(new QPidroResultCard);

    /*return*/ deliverEvent(new QPidroCommandPlayCard(sResult, suit));
    return Player::play_card(suit);
}

bool PlayerProxy::deliverEvent(QPidroEventT<bool>* pEvent)
{
    shared_ptr<QPidroResultBool> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}

int PlayerProxy::deliverEvent(QPidroEventT<int>* pEvent)
{
    shared_ptr<QPidroResultInt> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}

Pidro::Card* PlayerProxy::deliverEvent(QPidroEventT<Pidro::Card*>* pEvent)
{
    shared_ptr<QPidroResultCard> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->value();
}
