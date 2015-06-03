#include "qpidroevent.h"
#include <pidro/log.h>
#include "qpidroresult.h"
#include "qplayer.h"

using namespace std;

/*
 * QPidroEvent
 */
QPidroEvent::QPidroEvent()
    : QEvent(static_cast<QEvent::Type>(QPidroEvent::Id))
{
}

QPidroEvent::~QPidroEvent()
{
}

/*
 * QPidroInfoEvent
 */
QPidroInfoEvent::QPidroInfoEvent(std::shared_ptr<QPidroResultBool> sResult, Pidro::Event event)
    : QPidroEventT<bool>(sResult)
    , m_event(event)
{
}

void QPidroInfoEvent::deliverTo(QPlayer& player)
{
    result()->setValue(doDeliverTo(player));
}

/*
 * QPidroInfoEvent1
 */
QPidroInfoEvent1::QPidroInfoEvent1(shared_ptr<QPidroResultBool> sResult, Pidro::Event event)
    : QPidroInfoEvent(sResult, event)
{
}

bool QPidroInfoEvent1::doDeliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event);
}

/*
 * QPidroInfoEvent2
 */
QPidroInfoEvent2::QPidroInfoEvent2(shared_ptr<QPidroResultBool> sResult, Pidro::Event event, int position)
    : QPidroInfoEvent(sResult, event)
    , m_position(position)
{
}

bool QPidroInfoEvent2::doDeliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position);
}

/*
 * QPidroInfoEvent3
 */
QPidroInfoEvent3::QPidroInfoEvent3(shared_ptr<QPidroResultBool> sResult,
                                   Pidro::Event event, int position, int value)
    : QPidroInfoEvent(sResult, event)
    , m_position(position)
    , m_value(value)
{
}

bool QPidroInfoEvent3::doDeliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position, m_value);
}

/*
 * QPidroInfoEvent4
 */
QPidroInfoEvent4::QPidroInfoEvent4(shared_ptr<QPidroResultBool> sResult,
                                   Pidro::Event event,
                                   int position,
                                   list<Pidro::Card*>& cards) // TODO: Can be const?
    : QPidroInfoEvent(sResult, event)
    , m_position(position)
    , m_cards(cards)
{
}

bool QPidroInfoEvent4::doDeliverTo(QPlayer& player)
{
    LOG_D("Now should deliver!");

    return player.inform_event(m_event, m_position, m_cards);
}

/*
 * QPidroCommandGiveBid
 */
QPidroCommandGiveBid::QPidroCommandGiveBid(std::shared_ptr<QPidroResultInt> sResult,
                                           int minimum)
    : QPidroEventT<int>(sResult)
    , m_minimum(minimum)
{
}

void QPidroCommandGiveBid::deliverTo(QPlayer& player)
{
    result()->setValue(player.give_bid(m_minimum));
}

/*
 * QPidroCommandGiveColor
 */
QPidroCommandGiveColor::QPidroCommandGiveColor(std::shared_ptr<QPidroResultInt> sResult)
    : QPidroEventT<int>(sResult)
{
}

void QPidroCommandGiveColor::deliverTo(QPlayer& player)
{
    result()->setValue(player.give_color());
}

/*
 * QPidroCommandPlayCard
 */
QPidroCommandPlayCard::QPidroCommandPlayCard(std::shared_ptr<QPidroResultCard> sResult,
                                             int color)
    : QPidroEventT<Pidro::Card*>(sResult)
    , m_color(color)
{
}

void QPidroCommandPlayCard::deliverTo(QPlayer& player)
{
    result()->setValue(player.play_card(m_color));
}
