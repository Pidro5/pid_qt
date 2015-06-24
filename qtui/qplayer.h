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
        GAME_INIT          = static_cast<unsigned int>(Pidro::Event::GAME_INIT),          //  0
        ROUND_INIT         = static_cast<unsigned int>(Pidro::Event::ROUND_INIT),         //  1
        DEAL_CARD          = static_cast<unsigned int>(Pidro::Event::DEAL_CARD),          //  2
        ASK_FOR_BID        = static_cast<unsigned int>(Pidro::Event::ASK_FOR_BID),        //  3
        BID_PLACED         = static_cast<unsigned int>(Pidro::Event::BID_PLACED),         //  4
        GOT_BID            = static_cast<unsigned int>(Pidro::Event::GOT_BID),            //  5
        COLOR_SELECTED     = static_cast<unsigned int>(Pidro::Event::COLOR_SELECTED),     //  6
        PUT_CARDS_ON_TABLE = static_cast<unsigned int>(Pidro::Event::PUT_CARDS_ON_TABLE), //  7
        KILL_CARD          = static_cast<unsigned int>(Pidro::Event::KILL_CARD),          //  8
        BEGIN_PLAY         = static_cast<unsigned int>(Pidro::Event::BEGIN_PLAY),         //  9
        PLAY_CARD          = static_cast<unsigned int>(Pidro::Event::PLAY_CARD),          // 10
        PLAY_ROUND_FINISH  = static_cast<unsigned int>(Pidro::Event::PLAY_ROUND_FINISH),  // 11
        PLAYER_COLD        = static_cast<unsigned int>(Pidro::Event::PLAYER_COLD),        // 12
        ROUND_OVER         = static_cast<unsigned int>(Pidro::Event::ROUND_OVER),         // 13
        GAME_OVER          = static_cast<unsigned int>(Pidro::Event::GAME_OVER)           // 14
    };
    Q_ENUMS(Event)

    QString hello() const
    {
        return "Hello World!";
    }

    Q_INVOKABLE void setIntResult(int);

signals:
    void event1(Event event);
    void event2(Event event, int position);
    void event3(Event event, int position, int value);
    void event4(Event event, int position, const QVariantList& cards);

    void giveBid(int minimum);
    void giveSuit();

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
    void give_suit(std::shared_ptr<QPidroResultInt> sResult);
    Pidro::Card* play_card(int color);

public slots:

private:
    std::shared_ptr<QPidroResultInt> m_sIntResult;
};

#endif // QPLAYER_H
