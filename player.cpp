#include <iostream>
#include "log.h"
#include "player.h"
#include "game.h"

#include <cstdlib>

Player::Player(string strname)
{
    my_name = strname;
    my_position = -1;    // construct the player with a position negative = not attached to game

}

Player::~Player()
{

}

string Player::get_name(){
    return my_name;
}

void Player::attached_to_game(Game * g, int position, bool rotate_to_south)
{
    my_game = g;
    my_position = position;
    my_rotate_to_south = rotate_to_south;

}

bool Player::inform_event(event_type et){
    // this event gets
    // GAME_INIT
    // BEGIN_PLAY           just before the play begins - clear tables
    // PLAY_ROUND_FINISH   - when each player has been asked to play one card
                            //  after this the player with the highest card starts playing the next round..
                            //  This is good for showing some imtermediate stats
    // ROUND_OVER     -    all 14 cards played. The stats have been summed up
    // GAME_OVER

    return true;    //  true means continue game
}


bool Player::inform_event(event_type et, int position){
    // this event gets
    // ROUND_INIT
    // ASK_FOR_BID   - this tells who is bidding
    return true;    //  true means continue game
}

bool Player::inform_event(event_type et, int position, int value){
    // this event gets
    // DEAL_CARD  - when sombedy else gets cards dealt into their hand
    // BID_PLACED  - who(position), the bid (value )
    // GOT_BID  - who(position), the bid (value )
    // COLOR_SELECTED
    return true;    //  true means continue game
}


bool Player::inform_event(event_type et, int position, list<Card *>& cards){
    // this event gets
    // DEAL_CARD  -  I GOT CARDS dealt to my hand
    // PUT_CARDS_ON_TABLE    - somebody put one or more cards on the table..
    // KILL_CARD           - somebody had more than 6 cards and needs to kill a card
    // PLAY_CARD
    // PLAYER_COLD

    return true;    //  true means continue game
}


int Player::give_bid(int minimum){

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

int Player::give_color(){
    // color can be 0 ..3
    int rnd = rand() % 4;

    return rnd;
}

Card* Player::play_card(int color){
    list<Card *> tmp_list_of_cards = my_game->get_my_cards_in_hand(this);
    list<Card *>::iterator it;

    // Find out how many
    int how_many =0;
    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
        if ((*it)->card_ranking(color) != 0 ){
            how_many++;
        }
    }


    int rnd = rand() % how_many + 1;


    // Use the iterator to pick the card
    int i =0;
    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
        if ((*it)->card_ranking(color) != 0 ){
            ++i;
        }
        if (i == rnd){ return *it;}
    }
    return tmp_list_of_cards.front();    // then take the first one even if wrong..
}


