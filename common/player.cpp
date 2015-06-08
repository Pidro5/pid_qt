#include <pidro/player.h>
#include <cstdlib>
#include <iostream>
#include <pidro/log.h>
#include <pidro/game.h>

using namespace std;

namespace Pidro
{
  
Player::Player(const string &name)
    : m_name(name)
    , m_pGame(nullptr)
    , m_position(-1) // not attached to game
    , m_rotate_to_south(false)
{
}

Player::~Player()
{
}

string Player::get_name() const
{
    return m_name;
}

void Player::attached_to_game(Game* pGame, int position, bool rotate_to_south)
{
    m_pGame = pGame;
    m_position = position;
    m_rotate_to_south = rotate_to_south;

    LOG_D(get_name() << " was attached to game.. position: " << m_position);
}

int Player::give_bid(int minimum)
{

    // minimum is the currect higest bid
    // if 0 or -1  the next highest bid is 6
    // if minimum is 14, the player can exceed this with a bid of 14

    if (minimum ==5) {return 6;}

    int rnd = rand() % 10;
    if (rnd < 5) {return 0;}     //  50% to pass

    if (minimum ==0) {return 10;}
    if (minimum ==14) {return 14;}
    return minimum+1;
}

Card::Suit Player::give_suit()
{
    // color can be 0 ..3
    int rnd = rand() % 4;

    return static_cast<Card::Suit>(rnd);
}

Card* Player::play_card(Card::Suit suit)
{
    list<Card *> tmp_list_of_cards = m_pGame->get_my_cards_in_hand(this);
    list<Card *>::iterator it;

    // Find out how many
    int how_many =0;
    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
        if ((*it)->card_ranking(suit) != 0 ){
            how_many++;
        }
    }


    int rnd = rand() % how_many + 1;


    // Use the iterator to pick the card
    int i =0;
    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
        if ((*it)->card_ranking(suit) != 0 ){
            ++i;
        }
        if (i == rnd){ return *it;}
    }
    return tmp_list_of_cards.front();    // then take the first one even if wrong..
}

}

