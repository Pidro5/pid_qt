#ifndef PLAYERCOMPUTER_H
#define PLAYERCOMPUTER_H

#include <memory>
#include "player.h"
#include "playengine.h"

class BidEngine;

class PlayerComputer : public Player
{
public:
    PlayerComputer();
    PlayerComputer(const string& str);
    PlayerComputer(const PlayerComputer&) = delete;
    PlayerComputer& operator = (const PlayerComputer&) = delete;
    ~PlayerComputer();

    int give_bid(int minimum);
    int give_color();
    Card* play_card(int color);

    // this is a callback function and is called from the game after succesful registration
    void attached_to_game(Game* pGame, int position, bool rotate_to_south);

private:
    std::shared_ptr<BidEngine> m_sBidMachine;
    std::shared_ptr<PlayEngine> m_sPlayMachine;

};

#endif // PLAYERCOMPUTER_H
