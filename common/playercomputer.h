#ifndef PLAYERCOMPUTER_H
#define PLAYERCOMPUTER_H

#include <memory>
#include "player.h"
#include "playengine.h"

namespace Pidro
{
  
class BidEngine;

class PlayerComputer : public Player
{
public:
    PlayerComputer();
    PlayerComputer(const std::string& str);
    PlayerComputer(const PlayerComputer&) = delete;
    PlayerComputer& operator = (const PlayerComputer&) = delete;
    ~PlayerComputer();

    int give_bid(int minimum);
    int give_color();
    Card* play_card(int color);

    // this is a callback function and is called from the game after succesful registration
    void attached_to_game(Game* pGame, int position, bool rotate_to_south);

    bool inform_event(Event et) override;
    bool inform_event(Event et, int position) override;
    bool inform_event(Event et, int position, int value) override;
    bool inform_event(Event et, int position, std::list<Card *>& cards) override;

private:
    std::shared_ptr<BidEngine> m_sBidMachine;
    std::shared_ptr<PlayEngine> m_sPlayMachine;

};

}

#endif // PLAYERCOMPUTER_H
