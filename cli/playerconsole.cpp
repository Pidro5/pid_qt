#include "playerconsole.h"
#include <stdlib.h>
#include <pidro/game.h>
#include <pidro/log.h>

using namespace std;
using namespace Pidro;

PlayerConsole::PlayerConsole(const string& name)
    : Player(name)
    , m_who_has_deck(0)
{
}

PlayerConsole::~PlayerConsole()
{
}

void PlayerConsole::attached_to_game(Game* pGame, int position, bool rotate_to_south)
{
    Player::attached_to_game(pGame, position, rotate_to_south);
}


bool PlayerConsole::is_this_me(int position){
    if (m_rotate_to_south){
        if (position == 0){
            return true;
        }
    }
    else {
        if (m_position == position) {
            return true;
        }
    }
    return false;
}



bool PlayerConsole::inform_event(Event et){
    if (et == Event::GAME_INIT) {    }
    if (et == Event::BEGIN_PLAY) {

        for (int i =0; i<4; i++){
            m_my_view_on_tables[i] ="";
        }
        draw_table();
    }
    if (et == Event::PLAY_ROUND_FINISH) {
       // cout <<  "We got: " << my_game->get_round_our_points(this, 6) << " points ";
       // cout <<  "They got: " << my_game->get_round_their_points(this, 6) << " points " << endl<< endl;
        cin.get();
    }
    if (et == Event::ROUND_OVER) {
       cout <<  " Our Points: " << m_pGame->get_roundstat_our_points(this);
       cout <<  "  Their Points: " << m_pGame->get_roundstat_their_points(this);
       cout <<  " BID: " << m_pGame->get_roundstat_bid();
       cout <<  "  Who: "<< m_pGame->get_roundstat_name();
       cout <<  "  sits where: "<< m_pGame->get_roundstat_who_played(this) << endl<< endl;
       cin.get();

    }
    if (et == Event::GAME_OVER) {
        if ( m_pGame->get_game_winner(this)) {
            cout <<  "WE WON !!!";
        }
        else {
            cout <<  "We lost - they won";
        }
        cin.get();
    }

    return true;    //  true means continue game
}


bool PlayerConsole::inform_event(Event et, int position){
    if (et == Event::ROUND_INIT) {
        // INIT my internal DATA
        m_who_has_deck = position;
        for (int i =0; i<4; i++){
            m_my_view_on_hands[i] ="";
            m_my_view_on_tables[i] ="";
            m_my_view_on_discarded_cards[i]="";
        }
    }
    if (et == Event::ASK_FOR_BID) {
        m_my_view_on_tables[position] += "BIDS";
    }

    draw_table();    
    cin.get();

    return true;    //  true means continue game

}

bool PlayerConsole::inform_event(Event et, int position, int value){

    if (et == Event::DEAL_CARD) {
        for (int i =0; i<value; i++){
            m_my_view_on_hands[position] +=" XX";
        }
    }

    if (et == Event::BID_PLACED) {
        if (value == 0) {
            m_my_view_on_tables[position] += " -PASS";
        }
        else {
            char buffer [3];
            itoa(value,buffer,10);
            m_my_view_on_tables[position] += " ";
            m_my_view_on_tables[position] += buffer;
        }
    }

    if (et == Event::GOT_BID) {

        char buffer [3];
        itoa(value,buffer,10);
        m_my_view_on_tables[position] = " Got Bid: ";
        m_my_view_on_tables[position] += buffer;

    }


    if (et == Event::COLOR_SELECTED) {

        char buffer [3];
        itoa(value,buffer,10);
        m_my_view_on_tables[position] += " Color Selected: ";
        m_my_view_on_tables[position] += buffer;

    }
    draw_table();


    cin.get();
    return true;    //  true means continue game

}


bool PlayerConsole::inform_event(Event et, int position, list<Card *>& cards){

    if (et == Event::DEAL_CARD) {

        list<Card *>::iterator it;
        for (it = cards.begin(); it!=cards.end(); ++it) {
            m_my_view_on_hands[position] += ((*it)->card_suit_short()  + "_"+ (*it)->card_face_value()+ " ");
        }

    }

    if (et == Event::PUT_CARDS_ON_TABLE) {

        list<Card *>::iterator it;
        m_my_view_on_tables[position]="";   // clear info on the table first
        for (it = cards.begin(); it!=cards.end(); ++it) {
            m_my_view_on_tables[position] += ((*it)->card_suit_short()  + "_"+ (*it)->card_face_value()+ " ");
        }
    }

    if (et == Event::PLAY_CARD) {

        list<Card *>::iterator it;
       // my_view_on_tables[position]="";   // clear info on the table first
        for (it = cards.begin(); it!=cards.end(); ++it) {
            m_my_view_on_tables[position] += ((*it)->card_suit_short()  + "_"+ (*it)->card_face_value()+ " ");
        }
        cin.get();
    }

    if (et == Event::KILL_CARD) {

        list<Card *>::iterator it;
        m_my_view_on_discarded_cards[position]="KILL: ";   // clear info on the table first
        for (it = cards.begin(); it!=cards.end(); ++it) {
            m_my_view_on_discarded_cards[position] += ((*it)->card_suit_short()  + "_"+ (*it)->card_face_value()+ " ");
        }
    }

    if (et == Event::PUT_CARDS_ON_TABLE ||
            et == Event::KILL_CARD ||
            et == Event::PLAY_CARD ||
            et == Event::PLAYER_COLD) {

        // I need to refresh the remaining cards in hand
        m_my_view_on_hands[position] = "";
        if (!is_this_me(position)){   // somebody else's hand
            for (int i =0; i<m_pGame->get_how_many_cards_in_hand_pos_relative_to_me(this, position); i++){
                m_my_view_on_hands[position] +=" XX";
            }
        }
        else {
            m_my_view_on_hands[position] = "";

            list<Card *>::iterator it;
            list<Card *> mycards = m_pGame->get_my_cards_in_hand(this);
            for (it = mycards.begin(); it!=mycards.end(); ++it) {
                m_my_view_on_hands[position] += ((*it)->card_suit_short()  + "_"+ (*it)->card_face_value()+ " ");
            }
        }

    }

    draw_table();
      cin.get();
    if (et == Event::PLAY_CARD ||  et == Event::PLAYER_COLD) {
    //    LOG_D(*m_my_game);

    }
    return true;    //  true means continue game

}


int PlayerConsole::give_bid(int minimum){

    // minimum is the currect higest bid
    // if 0 or -1  the next highest bid is 6
    // if minimum is 14, the player can exceed this with a bid of 14

    int mybid;

    if (minimum == 0) {
        cout << "The highest bid is: PASS " << endl;
    }
    else {
        cout << "The highest bid is: " << minimum << endl;
    }
    cout << "What is your bid? " <<  endl;
    cout << "(Enter -1 to quit the game)? ";

    cin >> mybid;
    if (mybid < 0){m_pGame->quit_game();}
    return mybid;

}

int PlayerConsole::give_color(){

    // color can be 0 ..3

    int mybid;

    cout << "Chose your color: 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds? " <<  endl;
    cout << "(Enter -1 to quit the game)? ";

    cin >> mybid;
    if (mybid < 0){m_pGame->quit_game();}
    return mybid;

}

Card* PlayerConsole::play_card(int /*color*/){
    int mycard;
    list<Card *> tmp_list_of_cards = m_pGame->get_my_cards_in_hand(this);
    list<Card *>::iterator it;

    cout << "Which card to play? 1..6 " <<  endl;
    cout << "(Enter -1 or a number larger than the cards to quit the game)? ";

    cin >> mycard;
    if (mycard < 0){
        m_pGame->quit_game();
        return tmp_list_of_cards.front();
    }

    int i=1;
    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
        if (i == mycard){
            return *it;
        }
        i++;
    }
    return tmp_list_of_cards.front();
}

void PlayerConsole::draw_table(){
    system("cls");
    cout << "______________________________________________________________     " + get_name() << endl;
    if (m_who_has_deck == 2){
        cout << "                             <DECK> "<< endl;
    }
    cout << "                     " + m_my_view_on_hands[2]<< endl;
    cout << "                     " + m_my_view_on_tables[2]<< endl;
    cout << "                     " + m_my_view_on_discarded_cards[2]<< endl;

    for (int i =0; i < 3; i++){cout << endl;}
    if (m_who_has_deck == 1){
        cout << "<DECK> "<< endl;
    }

    cout <<  m_my_view_on_hands[1]<< endl;
    cout <<  m_my_view_on_tables[1]<< endl;
    cout <<  m_my_view_on_discarded_cards[1]<< endl;
    cout <<  "                                            "  + m_my_view_on_hands[3]<< endl;
    cout <<  "                                            "  + m_my_view_on_tables[3]<< endl;
    cout <<  "                                            "  + m_my_view_on_discarded_cards[3]<< endl;
    if (m_who_has_deck == 3){
        cout << "                                                                 <DECK> "<< endl;
    }


    for (int i =0; i < 3; i++){cout << endl;}


    cout << "                 " + m_my_view_on_hands[0]<< endl;
    cout << "                 " + m_my_view_on_tables[0]<< endl;
    cout << "                 " + m_my_view_on_discarded_cards[0]<< endl;
    if (m_who_has_deck ==0){
        cout << "                             <DECK> "<< endl;
    }
 //   cout << "___________________________________________________________________________"<< endl;
    cout << "_______________________________________________________________   PRESS ENTER"<< endl;
//    cout << "PRESS ENTER"<< endl;
}


