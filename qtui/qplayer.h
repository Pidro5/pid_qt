#ifndef QPLAYER_H
#define QPLAYER_H

#include <QObject>
#include <string>
#include <pidro/player.h>

class QPidroEvent;

class QPlayer : public QObject
{
    Q_OBJECT

public:
    explicit QPlayer(QObject* pParent = 0);
    ~QPlayer();

    bool event(QEvent* pEvent);
    bool event(QPidroEvent* pEvent);

    bool inform_event(Pidro::Event et);
    bool inform_event(Pidro::Event et, int position);
    bool inform_event(Pidro::Event et, int position, int value);
    bool inform_event(Pidro::Event et, int position, std::list<Pidro::Card*>& cards);

    int give_bid(int minimum);
    int give_color();
    Pidro::Card* play_card(int color);

signals:

public slots:
};

#endif // QPLAYER_H
