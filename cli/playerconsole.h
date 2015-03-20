#ifndef PLAYERCONSOLE_H
#define PLAYERCONSOLE_H

#include "common/player.h"

class PlayerConsole : public Player {
    // these variable are used to access the game and tells in which position the player is in the game
    //Game* my_game;
    //int  my_position;
    //bool my_rotate_to_south;

    int m_who_has_deck;
    string m_my_view_on_hands[4];
    string m_my_view_on_tables[4];
    string m_my_view_on_discarded_cards[4];

    bool is_this_me(int position);
    void draw_table();

public:
    PlayerConsole(string str);
    ~PlayerConsole();

    void attached_to_game(Game * g, int position, bool rotate_to_south);

    // these methods are called when the Player should update his UI
    bool inform_event(Event et);
    bool inform_event(Event et, int position);
    bool inform_event(Event et, int position, int value);
    bool inform_event(Event et, int position, list<Card *>& cards);

    int give_bid(int minimum);
    int give_color();
    Card* play_card(int color);

};

#endif // PLAYERCONSOLE_H
