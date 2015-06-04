#include <pidro/deck_card.h>
#include <cassert>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <pidro/log.h>


using namespace std;

namespace Pidro
{
  
Card::Card(int suit, int value)
{
    m_card_points =0;
    m_suit = suit;
    m_value = value;

    switch (value) {
    case 14:
    case 11:
    case 10:
        m_card_points = 1;
        break;

    case 5:
        m_card_points = 5;
        break;

    default:
        break;
    }

    // which card is higher A=14,K=13,better Pidro=5, lower pidro=4, (4,3,2= 3,2,1)
    m_ranking = value;

    if (value <= 4){
        m_ranking = value - 1; //(cards 4,3,2 =  ranking 3,2,1)
    }
}

string Card::card_face_value() const
{
    switch (m_value)
    {
    case 11:
        return "J";

    case 12:
        return "Q";

    case 13:
        return "K";

    case 14:
        return "A";

    case 5:
        return "V";

    default:
        // TODO: Add assert?
        return std::to_string(m_value);
    }
}

string Card::card_suit() const
{
    // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    switch (m_suit) {
    case 0:
        return "Spades";

    case 1:
        return "Clubs";

    case 2:
        return "Hearts";

    case 3:
        return "Diamonds";

    default:
        assert(!true);
        return "";
    }
}

string Card::card_suit_short() const
{
    // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    switch (m_suit) {
    case 0:
        return "SP";

    case 1:
        return "CL";

    case 2:
        return "HZ";

    case 3:
        return "DI";

    default:
        assert(!true);
        return "";
    }
}

int Card::card_ranking(int suit) const
{

    int ret_val = 0;

    if (m_value != 5){
        if (m_suit == suit) {
            ret_val = m_ranking;
        } else {
            ret_val = 0;
        }
    } else if(m_suit == suit){
        ret_val = 5;
    } else {
        // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
        if (m_suit == 0 and suit == 1) {
            ret_val = 4;
        } else if (m_suit == 1 and suit == 0) {
            ret_val = 4;
        } else if (m_suit == 2 and suit == 3) {
            ret_val = 4;
        } else if (m_suit == 3 and suit == 2) {
            ret_val = 4;
        }
    }

    return ret_val;
}


string Card::card_ranking_name(int suit) const
{
    string ret_val = "";

    if (m_value != 5) {
        if (m_suit == suit) {
            ret_val = this->card_face_value();
        } else {
            ret_val = "";
        }
    } else if(m_suit == suit){
        ret_val = "V";
    } else {
        // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
        if (m_suit == 0 and suit == 1) {
            ret_val = "v";
        } else if (m_suit == 1 and suit == 0) {
            ret_val = "v";
        } else if (m_suit == 2 and suit == 3) {
            ret_val = "v";
        } else if (m_suit == 3 and suit == 2) {
            ret_val = "v";
        }
    }

    return ret_val;
}

void Card::print(ostream& o) const
{
    o << this->card_suit_short() << "_" << this->card_face_value() << " ";
}

string Card::convert_rank_value_to_string(int value)
{
    string str = "";

    switch (value)
    {
    case 1:
        str = "2";
        break;
    case 2:
        str = "3";
        break;
    case 3:
        str = "4";
        break;
    case 4:
        str = "v";
        break;
    case 5:
        str = "V";
        break;
    case 6:
        str = "6";
        break;
    case 7:
        str = "7";
        break;
    case 8:
        str = "8";
        break;
    case 9:
        str = "9";
        break;
    case 10:
        str = "10";
        break;
    case 11:
        str = "J";
        break;
    case 12:
        str = "Q";
        break;
    case 13:
        str = "K";
        break;
    case 14:
        str = "A";
        break;


    default:
        throw std::runtime_error("Card::convert_rank_value_to_string - value not allowed");
    }
    return str;
}

int Card::convert_rank_name_to_value(const string& str)
{

    if (str == "2") { return 1; }
    if (str == "3") { return 2; }
    if (str == "4") { return 3; }
    if (str == "v") { return 4; }
    if (str == "V") { return 5; }
    if (str == "6") { return 6; }
    if (str == "7") { return 7; }
    if (str == "8") { return 8; }
    if (str == "9") { return 9; }
    if (str == "10") { return 10;}
    if (str == "J") { return 11; }
    if (str == "Q") { return 12; }
    if (str == "K") { return 13; }
    if (str ==  "A") { return 14;}
    throw std::runtime_error("convert_rank_name_to_value - string not allowed");
}


//
//  DECK ==============================================================================


Deck::Deck()
{
    int i = 0;
    int j = 0;

    for (i = 0; i < 4; i++) {
        for (j = 2; j <= 14; j++) {
            Card* pC = new Card(i, j);
            m_list_of_all_cards.push_back(pC);
        }
	}
}

Deck::~Deck()
{
    // Need to remove all cards from memory
    this->deck_delete();
}

void Deck::mix_deck()
{
    int rnd;
    list<Card *>::iterator it, it2;

    // empty mixed deck list if any cards
    while (!m_list_of_mixed_cards.empty()) {
        m_list_of_mixed_cards.pop_front();
    }

    /* initialize random seed: */
    srand(time(NULL));


    // iterate through all cards and insert to a random slot in mixedlist
    for (it = m_list_of_all_cards.begin(); it != m_list_of_all_cards.end(); ++it) {

        /* generate a number between 0 and mixed deck size */
        rnd = 0;
        if (m_list_of_mixed_cards.size() != 0) {   // if not 0 i can use modulo to calculate a number
            rnd = rand() % m_list_of_mixed_cards.size();
        }

        // Use the second iterator to iterate to position rnd

        it2 = m_list_of_mixed_cards.begin();
        for (int i = 0;i < rnd; ++i){
            ++it2;
        }
       m_list_of_mixed_cards.insert(it2,*it);
    }
}



int Deck::deal_cards_to_hand(int number, list<Card *>& hand)
{
    for (int i = 0; i < number; ++i) {
        if (m_list_of_mixed_cards.size() > 0) {
            hand.push_back(m_list_of_mixed_cards.front());
            m_list_of_mixed_cards.pop_front();
        } else {
            return i;
        }
    }

    return number;
}


void Deck::print(ostream& o) const {
    list<Card *>::const_iterator it;
    int i =0;

    for (it = m_list_of_mixed_cards.begin(); it!=m_list_of_mixed_cards.end(); ++it) {
       o << **it ;
       i++;
       if (i==10){
           i =0;
           o << endl;
       }
    }

    o << endl;
}


void Deck::deck_delete()
{
    while(!m_list_of_all_cards.empty()) {
        Card * pC;
        pC = m_list_of_all_cards.front();
        m_list_of_all_cards.pop_front();

        //LOG_D("Deleting card: " << c->card_suit() << " " << c->card_face_value());
        delete pC;
    }
}

}
