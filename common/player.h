#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "log.h"
#include "deck_card.h"
#include "event.h"

class Game;

class Player
{
public:
    Player(const std::string& name);
    Player(const Player&) = delete;
    Player& operator = (const Player&) = delete;
    virtual ~Player();

    std::string get_name() const;

    // this is a callback function and is called from the game after succesful registration
    virtual void attached_to_game(Game * g, int position, bool rotate_to_south);

    // These methods are called when the Player should update his UI

    // This event gets
    // GAME_INIT
    // BEGIN_PLAY           just before the play begins - clear tables
    // PLAY_ROUND_FINISH   - when each player has been asked to play one card
                            //  after this the player with the highest card starts playing the next round..
                            //  This is good for showing some imtermediate stats
    // ROUND_OVER     -    all 14 cards played. The stats have been summed up
    // GAME_OVER
    virtual bool inform_event(Event et) = 0;

    // This event gets
    // ROUND_INIT
    // ASK_FOR_BID   - this tells who is bidding
    virtual bool inform_event(Event et, int position) = 0;

    // This event gets
    // DEAL_CARD  - when sombedy else gets cards dealt into their hand
    // BID_PLACED  - who(position), the bid (value )
    // GOT_BID  - who(position), the bid (value )
    // COLOR_SELECTED
    virtual bool inform_event(Event et, int position, int value) = 0;

    // This event gets
    // DEAL_CARD  -  I GOT CARDS dealt to my hand
    // PUT_CARDS_ON_TABLE    - somebody put one or more cards on the table..
    // KILL_CARD           - somebody had more than 6 cards and needs to kill a card
    // PLAY_CARD
    // PLAYER_COLD
    virtual bool inform_event(Event et, int position, std::list<Card *>& cards) = 0;

    virtual int give_bid(int minimum);
    virtual int give_color();
    virtual Card* play_card(int color);

protected:
    // these variable are used to access the game and tells in which position the player is in the game
    std::string m_name;
    Game* m_pGame;
    int  m_position;
    bool m_rotate_to_south;
};

#endif // PLAYER_H
