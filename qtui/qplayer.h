#ifndef QPLAYER_H
#define QPLAYER_H

#include <QObject>
#include <memory>
#include <string>
#include <pidro/player.h>
#include "qpidroresult.h"

class QPidroEvent;

class QPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hello READ hello)

public:
    explicit QPlayer(QObject* pParent = 0);
    ~QPlayer();

    enum Event {
        GAME_INIT          = static_cast<unsigned int>(Pidro::Event::GAME_INIT),
        ROUND_INIT         = static_cast<unsigned int>(Pidro::Event::ROUND_INIT),
        DEAL_CARD          = static_cast<unsigned int>(Pidro::Event::DEAL_CARD),
        ASK_FOR_BID        = static_cast<unsigned int>(Pidro::Event::ASK_FOR_BID),
        BID_PLACED         = static_cast<unsigned int>(Pidro::Event::BID_PLACED),
        GOT_BID            = static_cast<unsigned int>(Pidro::Event::GOT_BID),
        COLOR_SELECTED     = static_cast<unsigned int>(Pidro::Event::COLOR_SELECTED),
        PUT_CARDS_ON_TABLE = static_cast<unsigned int>(Pidro::Event::PUT_CARDS_ON_TABLE),
        KILL_CARD          = static_cast<unsigned int>(Pidro::Event::KILL_CARD),
        BEGIN_PLAY         = static_cast<unsigned int>(Pidro::Event::BEGIN_PLAY),
        PLAY_CARD          = static_cast<unsigned int>(Pidro::Event::PLAY_CARD),
        PLAY_ROUND_FINISH  = static_cast<unsigned int>(Pidro::Event::PLAY_ROUND_FINISH),
        PLAYER_COLD        = static_cast<unsigned int>(Pidro::Event::PLAYER_COLD),
        ROUND_OVER         = static_cast<unsigned int>(Pidro::Event::ROUND_OVER),
        GAME_OVER          = static_cast<unsigned int>(Pidro::Event::GAME_OVER)
    };
    Q_ENUMS(Event)

    QString hello() const
    {
        return "Hello World!";
    }

    Q_INVOKABLE void setBid(int);

signals:
    void event1(Event event);
    void event2(Event event, int position);
    void event3(Event event, int position, int value);
    void event4(Event event, int position, const QVariantList& cards);

    void giveBid(int minimum);

public:
    static void declareQML();

public:
    bool event(QEvent* pEvent);
    bool event(QPidroEvent* pEvent);

    bool inform_event(Pidro::Event et);
    bool inform_event(Pidro::Event et, int position);
    bool inform_event(Pidro::Event et, int position, int value);
    bool inform_event(Pidro::Event et, int position, std::list<Pidro::Card*>& cards);

    void give_bid(int minimum, std::shared_ptr<QPidroResultInt> sResult);
    int give_color();
    Pidro::Card* play_card(int color);

public slots:

private:
    std::shared_ptr<QPidroResultInt> m_sIntResult;
};

#endif // QPLAYER_H
