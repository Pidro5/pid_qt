#include <time.h>
#include <iostream>
#include <cstdlib>
#include "deck_card.h"
#include "log.h"


using namespace std;


Card::Card(int suit, int value)
{
    card_points =0;
    mysuit = suit;
    myvalue = value;
    if (value==14){card_points = 1;};
    if (value==11){card_points = 1;};
    if (value==10){card_points = 1;};
    if (value==5){card_points = 5;};

    // which card is higher A=14,K=13,better Pidro=5, lower pidro=4, (4,3,2= 3,2,1)
    myranking = value;
    if (value <=4 ){ myranking = value - 1; //(cards 4,3,2 =  ranking 3,2,1)
    };
}

string Card::card_face_value()
{
    char buf[5];
    string str;

    sprintf(buf,"%d",myvalue);
    str = buf;

    if (myvalue == 11){str = "J";};
    if (myvalue == 12){str = "Q";};
    if (myvalue == 13){str = "K";};
    if (myvalue == 14){str = "A";};
    if (myvalue == 5){str = "V";};
   return str;

};

string Card::card_suit()
{
    // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    string str;
    str = "";
    if ( mysuit == 0){str = "Spades";};
    if ( mysuit == 1){str = "Clubs";};
    if ( mysuit == 2){str = "Hearts";};
    if ( mysuit == 3){str = "Diamonds";};
   return str;

};

string Card::card_suit_short()
{
    // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    string str;
    str = "";
    if ( mysuit == 0){str = "SP";};
    if ( mysuit == 1){str = "CL";};
    if ( mysuit == 2){str = "HZ";};
    if ( mysuit == 3){str = "DI";};
   return str;

};

int Card::card_ranking(int suit){

    int ret_val = 0;

    if (myvalue != 5){
        if (mysuit == suit)return myranking;
        else return 0;
    }
    else
        if(mysuit == suit){return 5;}
        else  {
            // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
            if (mysuit == 0 and suit == 1) {ret_val = 4;};
            if (mysuit == 1 and suit == 0) {ret_val = 4;};
            if (mysuit == 2 and suit == 3) {ret_val = 4;};
            if (mysuit == 3 and suit == 2) {ret_val = 4;};
        }

    return ret_val;
}


string Card::card_ranking_name(int suit){

    string ret_val = "";

    if (myvalue != 5){
        if (mysuit == suit)return this->card_face_value();
        else return "";
    }
    else
        if(mysuit == suit){return "V";}
        else  {
            // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
            if (mysuit == 0 and suit == 1) {ret_val = "v";};
            if (mysuit == 1 and suit == 0) {ret_val = "v";};
            if (mysuit == 2 and suit == 3) {ret_val = "v";};
            if (mysuit == 3 and suit == 2) {ret_val = "v";};
        }

    return ret_val;
}

//
//  DECK ==============================================================================


Deck::Deck()
{
    int i = 0;
    int j = 0;

    for (i = 0; i < 4;i++)
        for (j = 2; j <= 14;j++)
        {
            {
                Card * c = new Card(i,j);
                list_of_all_cards.push_back(c);
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
    while(!list_of_mixed_cards.empty())
    {
        list_of_mixed_cards.pop_front();
    }

    /* initialize random seed: */
    srand (time(NULL));


    // iterate through all cards and insert to a random slot in mixedlist
    for (it = list_of_all_cards.begin(); it!=list_of_all_cards.end(); ++it) {

        /* generate a number between 0 and mixed deck size */
        rnd = 0;
        if (list_of_mixed_cards.size()!= 0) {   // if not 0 i can use modulo to calculate a number
            rnd = rand() % list_of_mixed_cards.size();
        }

        // Use the second iterator to iterate to position rnd

        it2 = list_of_mixed_cards.begin();
        for (int i = 0;i < rnd; ++i){
            ++it2;
        }
       list_of_mixed_cards.insert(it2,*it);
    }
}



int Deck::deal_cards_to_hand(int number, list<Card *>& hand)
{
    for (int i = 0; i < number; ++i) {
        if (list_of_mixed_cards.size() > 0)
        {
            hand.push_back(list_of_mixed_cards.front());
            list_of_mixed_cards.pop_front();

        }
        else
        {
            return i;
        }


    }
    return number;

}



void Deck::TEST_TWEAK_mix_deck()
{

    list<Card *>::iterator it;

    // empty mixed deck list if any cards
    while(!list_of_mixed_cards.empty())
    {
        list_of_mixed_cards.pop_front();
    }




    // iterate through all cards
    for (int i = 0; i <3; i++) {
        for (it = list_of_all_cards.begin(); it!=list_of_all_cards.end(); ++it) {
            list_of_mixed_cards.push_back(*it);
        }
          for (int j = 0; j<40; j++) {
              list_of_mixed_cards.pop_back();
          }
    }
}



void Deck::deck_delete()
{
    while(!list_of_all_cards.empty())
    {
        Card * c;
        c = list_of_all_cards.front();
        list_of_all_cards.pop_front();

        //Log::instance().print("Deleting card: ");
        //Log::instance().println(c->card_suit() + " " + c->card_face_value());
        delete c;
    }
}


