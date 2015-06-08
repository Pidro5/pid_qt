#ifndef PIDRO_DECK_CARD_H
#define PIDRO_DECK_CARD_H

#include <iostream>
#include <list>

namespace Pidro
{
  
//  CARD ==============================================================================

class Card {
public:
    enum Suit {
        SPADES = 0,
        CLUBS = 1,
        HEARTS = 2,
        DIAMONDS = 3
    };

    static constexpr Suit Suits[] = { SPADES, CLUBS, HEARTS, DIAMONDS };

    Card(Suit suit, int value);
    Suit suit() const { return m_suit; }
    int value() const { return m_value; }
    std::string card_face_value() const;
    std::string card_suit() const;
    std::string card_suit_short() const;
    int card_ranking(Suit suit) const;
    std::string card_ranking_name(Suit suite) const;
    int m_card_points;            // A,J,10 - 1 point  5 - 5 points, 2- needs different handling
    void print(std::ostream& o) const;
    static std::string convert_rank_value_to_string(int value);
    static int convert_rank_name_to_value(const std::string& str);

private:
    int m_value;  // 2.. 14 (2,3,... 10,J,Q,K,A=14)
    Suit m_suit;  // 0..3, 0-Spades, 1-Clubs, 2-Hearts, 3-Diamonds
    int m_ranking; // which card is higher A=14,K=13,better Pidro=5, lower pidro=4, (4,3,2= 3,2,1)
    // Need something for sort order of the cards??
};

inline std::ostream& operator << (std::ostream& out, const Card& x) {
    x.print(out);
    return out;
}

//  DECK ==============================================================================

class Deck {
    std::list<Card *> m_list_of_all_cards;
    std::list<Card *> m_list_of_mixed_cards;

public:
    Deck ();
    ~Deck();

    void mix_deck();
    int cards_left_in_deck() const {return m_list_of_mixed_cards.size(); };
    int deal_cards_to_hand(int number, std::list<Card *>& hand);
    void deck_delete();
    void print(std::ostream& o) const;
};

inline std::ostream& operator << (std::ostream& out, const Deck& x) {
    x.print(out);
    return out;
}

}

#endif // DECK_CARD_H
