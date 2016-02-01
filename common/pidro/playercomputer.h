#ifndef PIDRO_PLAYERCOMPUTER_H
#define PIDRO_PLAYERCOMPUTER_H

#include <memory>
#include <pidro/player.h>
#include <pidro/playengine.h>

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
    Card::Suit give_suit();
    Card* play_card(Card::Suit suit);

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
