#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <iostream>
#include <list>

using namespace std;



//  CARD ==============================================================================

class Card {
    int m_myvalue;  // 2.. 14 (2,3,... 10,J,Q,K,A=14)
    int m_mysuit;  // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    int m_myranking; // which card is higher A=14,K=13,better Pidro=5, lower pidro=4, (4,3,2= 3,2,1)

    // Need something for sort order of the cards??

public:
    Card (int suite, int value);
    string card_face_value() const;
    string card_suit() const;
    string card_suit_short() const;
    int card_ranking(int suit) const;
    string card_ranking_name(int suite) const;
    int m_card_points;            // A,J,10 - 1 point  5 - 5 points, 2- needs different handling
    void print(ostream& o) const;
    static string convert_rank_value_to_string(int value);
    static int convert_rank_name_to_value(string str);

};

inline ostream& operator << (ostream& out, const Card& x) {
    x.print(out);
    return out;
}

//  DECK ==============================================================================

class Deck {
    list<Card *> m_list_of_all_cards;
    list<Card *> m_list_of_mixed_cards;

public:
    Deck ();
    ~Deck();

    void mix_deck();
    int cards_left_in_deck() const {return m_list_of_mixed_cards.size(); };
    int deal_cards_to_hand(int number, list<Card *>& hand);
    void deck_delete();
    void print(ostream& o) const;
};

inline ostream& operator << (ostream& out, const Deck& x) {
    x.print(out);
    return out;
}

#endif // DECK_CARD_H
