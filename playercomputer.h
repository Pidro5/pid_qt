#ifndef PLAYERCOMPUTER_H
#define PLAYERCOMPUTER_H
#include "player.h"


class PlayerComputer : public Player
{
public:
    PlayerComputer();
    PlayerComputer(string str);
    ~PlayerComputer();
    int give_bid(int minimum);
    int give_color();
    // this is a callback function and is called from the game after succesful registration
    void attached_to_game(Game * g, int position, bool rotate_to_south);


};

#endif // PLAYERCOMPUTER_H
