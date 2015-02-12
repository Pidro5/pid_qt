#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <iostream>
#include <list>

using namespace std;

//  CARD ==============================================================================

class Card {
    int myvalue;  // 2.. 14 (2,3,... 10,J,Q,K,A=14)
    int mysuit;  // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    int myranking; // which card is higher A=14,K=13,better Pidro=5, lower pidro=4, (4,3,2= 3,2,1)

    // Need something for sort order of the cards??

public:
    Card (int suite, int value);
    string card_face_value();
    string card_suit();
    string card_suit_short();
    int card_ranking(int suit);
    string card_ranking_name(int suite);
    int card_points; // A,J,10 - 1 point  5 - 5 points, 2- needs different handling

};

//  DECK ==============================================================================

class Deck {
    list<Card *> list_of_all_cards;
    list<Card *> list_of_mixed_cards;

public:
    Deck ();
    ~Deck();

    void mix_deck();
    int cards_left_in_deck() {return list_of_mixed_cards.size(); };
    int deal_cards_to_hand(int number, list<Card *>& hand);
    void deck_delete();

    void TEST_TWEAK_mix_deck();

};

#endif // DECK_CARD_H
