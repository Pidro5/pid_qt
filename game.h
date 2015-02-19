#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "deck_card.h"
#include "rotator.h"
#include "event.h"


class RoundRecord
{
public:
   int points_north_south;
   int points_east_west;
   int who_played;
   int what_was_the_bid;
   string name_of_player;
};

class Game
{

    //  The game has 4 positions of players
    //  0-South, 1-West, 2-North, 3-East

    Player * game_player[4];
    int no_of_players_registered;

    // each player may want to get ther data as if they were sitting in "0-South."
    // This would be the case for instance if all players played remotely on a server
    // All visual data given from the game will be modified to suit the players viepoint
    // initially 0-South = 0, 1-West  =1, 2-North = 2, 3-East = 3  - no transformation required

    bool turn_players_viewpoint_to_south[4];

    // ==================================================================================
    // ==================   GAME        DATA         ====================================
    // ==================================================================================

    bool b_continue_game;
    int game_north_south_points, game_east_west_points;

    list<RoundRecord *> game_round_records;

    bool b_winner_ns,b_winner_ew;


    // ==================================================================================
    // ==================   ROUND        DATA         ====================================
    // ==================================================================================

    Deck my_deck;
    list<Card *> cards_in_hands[4];   // view only from the player himself
    list<Card *> cards_on_table[4];
    list<Card *> cards_discarded[4];


    void delete_round_events();
    void post_event(Event et);
    void post_event(Event et, int position);
    void post_event(Event et, int position, int value);
    void post_event(Event et, int position, list<Card *>& cards);

    // each bid pro round. -1 if no bid placed
    int bids[4];
    int higest_bid;
    int who_has_higest_bid;
    int selected_color;
    bool check_if_bid_is_valid(int bid, int highbid);

    void throw_worthless_cards_on_the_table_do_not_keep_more_than_six(int who, int color, bool adjust_hand_to_six);
    int check_if_player_has_cards_to_play(int who_plays,int selected_color );  // returns the number of cards the player has


    // Rotators
    Rotator who_has_deck;
    Rotator who_bids;
    Rotator who_plays;

    // Play Data
    bool is_cold[4];

    //  ----------------------
    Card * played_cards[14];
    int  played_by_whom[14];
    int  played_in_round[14];
    int  played_idx;
    //  ----------------------

    void register_card_as_played(Card * c,int who, int round);
    int who_had_the_higest_card_in_round(int round, int color) const;
    int sum_round_points(bool NS_or_WE, int up_to_round) const;

    // ==================================================================================
    // ==================          OTHER             ====================================
    // ==================================================================================

    void game_init();
    void round_init();
    int authorise_player(Player * pl) const;

public:
    Game();
    ~Game();

    void register_with_game(Player * pl);
    void register_with_game(Player * pl, bool turn_my_view_to_the_south);
    void run_game();
    void quit_game();


    // ==================================================================================
    // ======================== Interface to player       +++++++++++++++++++++==========

    int get_game_our_points(Player * pl);   // VI
    int get_game_their_points(Player * pl);   // DE
    bool get_game_winner(Player * pl);      // If player is on the winning team - true. Else - false

    int get_round_our_points(Player * pl, int including_round) const;
    int get_round_their_points(Player * pl, int including_round) const;

    int get_roundstat_our_points(Player * pl);
    int get_roundstat_their_points(Player * pl);
    string get_roundstat_name();
    int get_roundstat_bid();
    int get_roundstat_who_played(Player * pl);

    int get_highest_bid_value();
    int get_bid_value_pos_absolute(int pos);
    // get players bid value, offset 0 - me, 1 - next, 2 -partner, 3 -just before me
    int get_bid_value_pos_relative_to_me(Player * pl, int offset);

    int get_who_has_deck_pos_absolute();
    int get_who_bids_pos_absolute();
    int get_who_plays_pos_absolute();

    list<Card *>  get_my_cards_in_hand(Player * pl);
    list<Card *>  get_cards_on_the_table(int pos);
    int get_how_many_cards_in_hand_pos_relative_to_me(Player * pl, int pos);


    // ==================================================================================

      void print(ostream& o) const;

};

inline ostream& operator << (ostream& out, const Game& x) {
    x.print(out);
    return out;
}
#endif // GAME_H
