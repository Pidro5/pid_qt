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

bool PlayerProxy::inform_event(Event ev)
{
    LOG_D(ev);

    shared_ptr<QPidroResult> sResult(new QPidroResult);

    return deliverEvent(new QPidroEvent1(sResult, ev));
}

bool PlayerProxy::inform_event(Event ev, int position)
{
    LOG_D(ev << " " << position);

    shared_ptr<QPidroResult> sResult(new QPidroResult);

    return deliverEvent(new QPidroEvent2(sResult, ev, position));
}

bool PlayerProxy::inform_event(Event ev, int position, int value)
{
    LOG_D(ev << " " << position << " " << value);

    shared_ptr<QPidroResult> sResult(new QPidroResult);

    return deliverEvent(new QPidroEvent3(sResult, ev, position, value));

}

bool PlayerProxy::inform_event(Event ev, int position, std::list<Card*>& cards)
{
    LOG_D(ev << " " << position << " " << "...");

    shared_ptr<QPidroResult> sResult(new QPidroResult);

    return deliverEvent(new QPidroEvent4(sResult, ev, position, cards));
}

bool PlayerProxy::deliverEvent(QPidroEvent* pEvent)
{
    shared_ptr<QPidroResult> sResult = pEvent->result();

    QCoreApplication::postEvent(m_pPlayer, pEvent);

    sResult->wait();

    return sResult->code();
}
