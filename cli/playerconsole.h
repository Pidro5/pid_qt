#ifndef PLAYERCONSOLE_H
#define PLAYERCONSOLE_H

#include <pidro/player.h>

class PlayerConsole : public Pidro::Player
{
    // these variable are used to access the game and tells in which position the player is in the game
    //Game* my_game;
    //int  my_position;
    //bool my_rotate_to_south;

    int m_who_has_deck;
    std::string m_my_view_on_hands[4];
    std::string m_my_view_on_tables[4];
    std::string m_my_view_on_discarded_cards[4];

    bool is_this_me(int position);
    void draw_table();

public:
    PlayerConsole(const std::string& str);
    ~PlayerConsole();

    void attached_to_game(Pidro::Game * g, int position, bool rotate_to_south);

    // these methods are called when the Player should update his UI
    bool inform_event(Pidro::Event et);
    bool inform_event(Pidro::Event et, int position);
    bool inform_event(Pidro::Event et, int position, int value);
    bool inform_event(Pidro::Event et, int position, std::list<Pidro::Card*>& cards);

    int give_bid(int minimum);
    Pidro::Card::Suit give_suit();
    Pidro::Card* play_card(int color);

};

#endif // PLAYERCONSOLE_H
