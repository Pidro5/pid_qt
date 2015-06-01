#ifndef PLAYERPROXY_H
#define PLAYERPROXY_H

#include <string>
#include "player.h"

class QPidroEvent;
class QPlayer;

class PlayerProxy : public Player
{
public:
    explicit PlayerProxy(const std::string& name, QPlayer* pPlayer);
    PlayerProxy(const PlayerProxy&) = delete;
    PlayerProxy& operator = (const PlayerProxy&) = delete;
    ~PlayerProxy();

private:
    bool inform_event(Event et) override;
    bool inform_event(Event et, int position) override;
    bool inform_event(Event et, int position, int value) override;
    bool inform_event(Event et, int position, std::list<Card *>& cards) override;

    bool deliverEvent(QPidroEvent* pEvent);

private:
    QPlayer* m_pPlayer;
};

#endif

