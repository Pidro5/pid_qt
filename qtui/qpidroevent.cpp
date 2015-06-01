#include "qpidroevent.h"
#include <log.h>
#include "qpidroresult.h"
#include "qplayer.h"

using namespace std;

/*
 * QPidroEvent
 */
QPidroEvent::QPidroEvent(shared_ptr<QPidroResult> sResult)
    : QEvent(static_cast<QEvent::Type>(QPidroEvent::Id))
    , m_sResult(sResult)
{
}

QPidroEvent::~QPidroEvent()
{
}

std::shared_ptr<QPidroResult> QPidroEvent::result() const
{
    return m_sResult;
}

/*
 * QPidroEvent1
 */
QPidroEvent1::QPidroEvent1(shared_ptr<QPidroResult> sResult, Pidro::Event event)
    : QPidroEvent(sResult)
    , m_event(event)
{
}

bool QPidroEvent1::deliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event);
}

/*
 * QPidroEvent2
 */
QPidroEvent2::QPidroEvent2(shared_ptr<QPidroResult> sResult, Pidro::Event event, int position)
    : QPidroEvent(sResult)
    , m_event(event)
    , m_position(position)
{
}

bool QPidroEvent2::deliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position);
}

/*
 * QPidroEvent3
 */
QPidroEvent3::QPidroEvent3(shared_ptr<QPidroResult> sResult, Pidro::Event event, int position, int value)
    : QPidroEvent(sResult)
    , m_event(event)
    , m_position(position)
    , m_value(value)
{
}

bool QPidroEvent3::deliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position, m_value);
}

/*
 * QPidroEvent4
 */
QPidroEvent4::QPidroEvent4(shared_ptr<QPidroResult> sResult,
                           Pidro::Event event,
                           int position,
                           list<Pidro::Card*>& cards) // TODO: Can be const?
    : QPidroEvent(sResult)
    , m_event(event)
    , m_position(position)
    , m_cards(cards)
{
}

bool QPidroEvent4::deliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position, m_cards);
}


