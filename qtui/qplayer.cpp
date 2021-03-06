#include "qplayer.h"
#include <cassert>
#include <iostream>
#include <QList>
#include <QPoint>
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

void QPlayer::setIntResult(int value)
{
    if (m_sIntResult) {
        m_sIntResult->setValue(value);
        m_sIntResult.reset();
    } else {
        LOG_E("Int-result was not available.");
    }
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

    emit event1(static_cast<Event>(et));

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

    emit event2(static_cast<Event>(ev), position);

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

    emit event3(static_cast<Event>(ev), position, value);

    return true;
}

bool QPlayer::inform_event(Pidro::Event ev, int position, std::list<Pidro::Card*>& cards)
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

    QVariantList qmlCards;

    transform(cards.begin(),
              cards.end(),
              std::back_inserter(qmlCards),
              [] (const Pidro::Card* pCard) {
                  QMap<QString, QVariant> item;
                  QString suit = QString::fromStdString(pCard->card_suit_short()).left(1);
                  item.insert("suit", QVariant(suit));
                  QString value = QString::fromStdString(pCard->card_face_value());
                  if (value == "V") {
                      value = "5";
                  }
                  item.insert("value", QVariant(value));
                  return item;
              });

    emit event4(static_cast<Event>(ev), position, qmlCards);

    return true;
}

void QPlayer::give_bid(int minimum, shared_ptr<QPidroResultInt> sResult)
{
    LOG_D("give_bid");
    m_sIntResult = sResult;
    emit giveBid(minimum);
}

void QPlayer::give_suit(shared_ptr<QPidroResultInt> sResult)
{
    LOG_D("give_color");
    m_sIntResult = sResult;
    emit giveSuit();
}

Pidro::Card* QPlayer::play_card(int color)
{
    LOG_D("play_card");
    return 0;
}
