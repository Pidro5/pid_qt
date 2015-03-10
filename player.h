#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "log.h"
#include "deck_card.h"
#include "event.h"

class Game;

class Player
{
    // these variable are used to access the game and tells in which position the player is in the game
protected:
    Game* m_my_game;
    int  m_my_position;
    bool m_my_rotate_to_south;

    string m_my_name;

public:
    Player(string str);
    ~Player();

    string get_name();

    // this is a callback function and is called from the game after succesful registration
    virtual void attached_to_game(Game * g, int position, bool rotate_to_south);

    // these methods are called when the Player should update his UI
    virtual bool inform_event(Event et);
    virtual bool inform_event(Event et, int position);
    virtual bool inform_event(Event et, int position, int value);
    virtual bool inform_event(Event et, int position, list<Card *>& cards);

    virtual int give_bid(int minimum);
    virtual int give_color();
    virtual Card* play_card(int color);


};

#endif // PLAYER_H
