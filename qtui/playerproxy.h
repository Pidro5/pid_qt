#ifndef PLAYERPROXY_H
#define PLAYERPROXY_H

#include <string>
#include <pidro/player.h>
#include "qpidroevent.h"

class QPlayer;

class PlayerProxy : public Pidro::Player
{
public:
    explicit PlayerProxy(const std::string& name, QPlayer* pPlayer);
    PlayerProxy(const PlayerProxy&) = delete;
    PlayerProxy& operator = (const PlayerProxy&) = delete;
    ~PlayerProxy();

private:
    bool inform_event(Pidro::Event et) override;
    bool inform_event(Pidro::Event et, int position) override;
    bool inform_event(Pidro::Event et, int position, int value) override;
    bool inform_event(Pidro::Event et, int position, std::list<Pidro::Card *>& cards) override;

    int give_bid(int minimum);
    int give_color();
    Pidro::Card* play_card(int color);

    bool deliverEvent(QPidroEventT<bool>* pEvent);
    int deliverEvent(QPidroEventT<int>* pEvent);
    Pidro::Card* deliverEvent(QPidroEventT<Pidro::Card*>* pEvent);

private:
    QPlayer* m_pPlayer;
};

#endif

