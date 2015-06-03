#include <pidro/game.h>
#include <cassert>
#include <stdexcept>
#include <pidro/log.h>

using namespace std;

namespace Pidro
{
  
Game::Game()
{
    //  Players and viewoints
    m_no_of_players_registered =0;


    for (int i = 0; i < 4; i++) {
        m_turn_players_viewpoint_to_south[i] = false;
    }

}

Game::~Game()
{
    // Game History delete
    while (!m_game_round_records.empty()) {
        RoundRecord* pRecord = m_game_round_records.front();
        m_game_round_records.pop_front();
        delete pRecord;
    }
}




void Game::register_with_game(Player * pl){
    register_with_game(pl,false);
}

void Game::register_with_game(Player * pl, bool turn_my_view_to_the_south){

    if (m_no_of_players_registered < 4){
        m_turn_players_viewpoint_to_south[m_no_of_players_registered] = turn_my_view_to_the_south;

        // Store the player
        m_pGame_player[m_no_of_players_registered] = pl;
        // Call the player to inform about registration
        m_pGame_player[m_no_of_players_registered]->attached_to_game(this,m_no_of_players_registered,turn_my_view_to_the_south);

        m_no_of_players_registered++;
    }
    else {
        LOG_E("Game::register_with_game - Too many players registered.");
        throw std::runtime_error("Too many players registered.");
    }

}

void Game::run_game() {
    Rotator r;
    bool b_game_over;

    if(m_no_of_players_registered !=4){

        // Not 4 players registered in this game - do nothing
        LOG_E("GAME: Not enough players - cannot start! ");

        return;
    }


    // Initial setting of deck position
    m_who_has_deck.set_position(0);
    m_who_has_deck--;              // Decrement to "-1" because before each round starts the deck moves ++

    m_bContinue_game = true;       // game should run

    while (m_bContinue_game){

        // ==========================================================================
        // Game init
        // ==========================================================================

        //LOG_D("GAME: Call Game Init ");
        game_init();
        post_event(Event::GAME_INIT);
        if(!m_bContinue_game){return;}

        b_game_over = false;     // we are still not finished with this one game

        while (!b_game_over){
            // ==========================================================================
            // Round init
            // ==========================================================================

            //LOG_D("GAME: Call Round Init ");
            round_init();
            post_event(Event::ROUND_INIT, m_who_has_deck.get_position());
            if(!m_bContinue_game){return;}


            // ==========================================================================
            // Now deal the cards 3 x 3 for each Player
            // ==========================================================================

            for (int j=0;j<3;j++){
                r = m_who_has_deck;
                list<Card *>::iterator it;
                list<Card *> tmp_list_of_cards;

                for (int i=0;i<4;i++){
                    r++; //deal startint wtih the next one


                    if(m_my_deck.deal_cards_to_hand(3,tmp_list_of_cards) != 3){
                        LOG_W("Something wrong with dealing cards.");
                        throw std::runtime_error("Something wrong with dealing cards.");
                    }
                    //tmp_list_of_cards  has 3 cards

                    //  add to hand
                    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                        m_cards_in_hands[r.get_position()].push_back(*it);
                    }
                    post_event(Event::DEAL_CARD, r.get_position(),tmp_list_of_cards);
                    if(!m_bContinue_game){return;}

                    // Empty the tmp list afterwards
                    while(!tmp_list_of_cards.empty()) {
                        tmp_list_of_cards.pop_front();

                    }
                }
            }
            // ==========================================================================
            // Now do the bidding
            // ==========================================================================
            m_who_bids = m_who_has_deck;
            m_who_has_higest_bid =-1;

            for (int i=0;i<4;i++){
                m_who_bids++;  // Start with the one after deck
                post_event(Event::ASK_FOR_BID, m_who_bids.get_position());
                if(!m_bContinue_game){return;}

                // call the players
                if (i == 3 && m_higest_bid == 0){    // if this is the last one and nobody bid anything
                    m_higest_bid = 5;
                    m_who_has_higest_bid = m_who_bids.get_position();  // the last one has the highest bid
                }

                // TRY 3 times
                // ============
                int i_try = 0;
                bool bid_ok = false;

                while (!bid_ok){
                    m_bids[m_who_bids.get_position()] = m_pGame_player[m_who_bids.get_position()]->give_bid(m_higest_bid);
                    if(!m_bContinue_game){return;}
                    i_try++;
                    // do some sanity check about the bid
                    bid_ok = check_if_bid_is_valid(m_bids[m_who_bids.get_position()], m_higest_bid);

                    if (!bid_ok && i_try>=3) {
		      LOG_W("Player "
			    << m_pGame_player[m_who_bids.get_position()]->get_name()
			    << " gave 3 wrong bids. Kick him!");
                        throw std::runtime_error("Player bid not allowed");
                    }
                }

                // ============
                // Bid OK - proceed
                // set new higest bid if it is higher

                if (m_bids[m_who_bids.get_position()] > m_higest_bid){
                    m_higest_bid = m_bids[m_who_bids.get_position()];
                    m_who_has_higest_bid = m_who_bids.get_position();
                }
                // if the bid was 14 - then set the later Player to having the highest bid
                if (m_bids[m_who_bids.get_position()] == 14){
                    m_who_has_higest_bid = m_who_bids.get_position();
                }

                // tell everybody about the Bid
                post_event(Event::BID_PLACED, m_who_bids.get_position(), m_bids[m_who_bids.get_position()]);
                if(!m_bContinue_game){return;}

            }  // ends the Bidding

            // ==========================================================================
            // Now inform about the winner
            // ==========================================================================

            post_event(Event::GOT_BID, m_who_has_higest_bid, m_higest_bid);
            if(!m_bContinue_game){return;}


            // ==========================================================================
            // Select color
            // ==========================================================================

            // TRY 3 times
            // ============
            int i_try = 0;
            bool bid_ok = false;

            while (!bid_ok){

                m_selected_color = m_pGame_player[m_who_has_higest_bid]->give_color();
                if(!m_bContinue_game){return;}
                i_try++;

                // do some sanity check about the color
                if (m_selected_color >=0 && m_selected_color <=3) {
                    bid_ok = true;
                }

                if (!bid_ok && i_try>=3) {
		    LOG_E("Player "
			  << m_pGame_player[m_who_has_higest_bid]->get_name()
			  << " gave 3 wrong colors. Kick him!");
                    throw std::runtime_error("Player color not allowed");
                }
            }

            post_event(Event::COLOR_SELECTED, m_who_has_higest_bid, m_selected_color);
            if(!m_bContinue_game){return;}

            // ==========================================================================
            // Hands (except the one holding the deck) throws excess cards on the table
            // ==========================================================================

            r = m_who_has_deck;

            for (int i=0;i<3;i++){    // Note - only 3 - until we reach the deck
                r++; //start with the next one after deck
                throw_worthless_cards_on_the_table_do_not_keep_more_than_six(r.get_position(),m_selected_color, false);

                // note how many visible cards they have after color selection
                m_how_many_cards_do_i_have[r.get_position()]=m_cards_in_hands[r.get_position()].size();
                m_how_many_visible_cards_has_player[r.get_position()]=m_cards_in_hands[r.get_position()].size();
                m_how_many_visible_cards_before_buy[r.get_position()]=m_cards_in_hands[r.get_position()].size();

            }




            // ==========================================================================
            // Deal to 6 cards
            // ==========================================================================

            r = m_who_has_deck;

            for (int i=0;i<3;i++){    // Note - only 3 - until we reach the deck

                r++; //start with the next one after deck
                list<Card *>::iterator it;
                list<Card *> tmp_list_of_cards;

                int how_many_cards =6 - m_cards_in_hands[r.get_position()].size();
                if (how_many_cards > 0) {
                    if(m_my_deck.deal_cards_to_hand(how_many_cards,tmp_list_of_cards) != how_many_cards){
                        LOG_E("Something wrong with dealing cards.");
                        throw std::runtime_error("Something wrong with dealing cards.");
                    }
                    //  add to hand
                    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                        m_cards_in_hands[r.get_position()].push_back(*it);

                        if ((*it)->card_ranking(m_selected_color)> 0){ // got one card
                            m_how_many_cards_do_i_have[r.get_position()]++;
                        }
                    }
                    post_event(Event::DEAL_CARD, r.get_position(),tmp_list_of_cards);
                    if(!m_bContinue_game){return;}

                    // Empty the tmp list afterwards
                    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }
                }
            }

            // ==========================================================================
            // Deal the deck-rest to the one who has the deck.
            // ==========================================================================
            list<Card *>::iterator it;
            list<Card *> tmp_list_of_cards;
            int how_many_cards = m_my_deck.cards_left_in_deck();
            if (how_many_cards > 0) {
                if(m_my_deck.deal_cards_to_hand(how_many_cards,tmp_list_of_cards) != how_many_cards){
                    LOG_E("Something wrong with dealing cards.");
                    throw std::runtime_error("Something wrong with dealing cards.");
                }
                //  add to hand
                for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                    m_cards_in_hands[m_who_has_deck.get_position()].push_back(*it);
                }
                post_event(Event::DEAL_CARD, m_who_has_deck.get_position(),tmp_list_of_cards);
                if(!m_bContinue_game){return;}

                // Empty the tmp list afterwards
                while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }
            }

            // I need to update how many cards the deck owner sees
            m_how_many_cards_do_i_have[m_who_has_deck.get_position()] = 0;

            for (it = m_cards_in_hands[m_who_has_deck.get_position()].begin(); it!=m_cards_in_hands[m_who_has_deck.get_position()].end(); ++it) {
                if ((*it)->card_ranking(m_selected_color)> 0){ // got one card - need to update my knowhow
                    m_how_many_cards_do_i_have[m_who_has_deck.get_position()]++;
                }
            }


            // ==========================================================================
            // The deck throws the unnecessary cards
            // ==========================================================================

            throw_worthless_cards_on_the_table_do_not_keep_more_than_six(m_who_has_deck.get_position(),m_selected_color,true);

            // ==========================================================================
            // PLAY begins
            // ==========================================================================

            post_event(Event::BEGIN_PLAY);
            if(!m_bContinue_game){return;}

            // ==========================================================================
            // PLAY
            // ==========================================================================

            m_who_plays.set_position(m_who_has_higest_bid);

            for (int i_round=1;i_round<=6;i_round++){  // 6 Rounds   1..6 (Round 0 reserved for Killed cards
                bool a_card_played = false;

                for (int i_player=0;i_player<4;i_player++){  // 4 player

                    // Check if cold
                    if(!m_player_is_cold[m_who_plays.get_position()] && check_if_player_has_cards_to_play(m_who_plays.get_position(), m_selected_color ) == 0){
                        // this player just got cold - needs to be registered

                        m_player_is_cold[m_who_plays.get_position()] = true;

                        // he needs to throw his card - out of the hand - but not onto the table
                        list<Card *> tmp_list_of_cards;

                        while(!m_cards_in_hands[m_who_plays.get_position()].empty()){
                            tmp_list_of_cards.push_back(m_cards_in_hands[m_who_plays.get_position()].front());
                            m_cards_in_hands[m_who_plays.get_position()].pop_front();
                        }

                        // Inform
                        post_event(Event::PLAYER_COLD, m_who_plays.get_position(),
                                   m_cards_in_hands[m_who_plays.get_position()]);
                        if(!m_bContinue_game){return;}

                        // Empty the tmp list afterwards
                        while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }

                    }

                    if(!m_player_is_cold[m_who_plays.get_position()]){

                        // player has still cards - then ask for a card

                        // TRY 3 times
                        // ============
                        int i_try = 0;
                        bool play_ok = false;
                        Card * c;

                        while (!play_ok){

                            c = m_pGame_player[m_who_plays.get_position()]->play_card(m_selected_color);
                            if(!m_bContinue_game){return;}
                            i_try++;

                            // do some sanity check
                            if (c->card_ranking(m_selected_color)> 0) {  // ok card
                                play_ok = true;
                            }

                            if (!play_ok && i_try>=3) {
			        LOG_W("Player "
				      << m_pGame_player[r.get_position()]->get_name()
				      << " played 3 wrong cards. Kick him!");
                                throw std::runtime_error("Player card not allowed");
                            }
                        }
                        // if ok play it
                        register_card_as_played(c,m_who_plays.get_position(), i_round);
                        a_card_played=true;

                        // remove card from hand - and  put it on the table
                        m_cards_in_hands[m_who_plays.get_position()].remove(c);


                        // post card play
                        list<Card *> tmp_list_of_cards;
                        tmp_list_of_cards.push_back(c);

                        post_event(Event::PLAY_CARD, m_who_plays.get_position(), tmp_list_of_cards);
                        if(!m_bContinue_game){return;}

                        // Empty the tmp list afterwards
                        while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }

                    }
                    m_who_plays++;   // next one in round

                }
                play_round_completed(i_round);

                // Check who has the higest card in round
                if(a_card_played) {post_event(Event::PLAY_ROUND_FINISH);}
                if(!m_bContinue_game){return;}

                m_who_plays.set_position(who_had_the_higest_card_in_round(i_round, m_selected_color));

            }

            // ==========================================================================
            // DONE - AFTERMATH
            // ==========================================================================

            // Store the result in a list
            RoundRecord * rr = new RoundRecord();
            m_game_round_records.push_back(rr);
            rr->m_who_played = m_who_has_higest_bid;
            rr->m_what_was_the_bid = m_higest_bid;
            rr->m_name_of_player = m_pGame_player[m_who_has_higest_bid]->get_name();

            int vi = sum_round_points(true, 6);
            int de = sum_round_points(false, 6);
            if (m_who_has_higest_bid == 0|| m_who_has_higest_bid ==2) // North South
            {
                if(vi < m_higest_bid) {
                    // then you lose
                    vi = - m_higest_bid;
                    rr->m_what_was_the_bid = vi;
                }
            }
            else
            {
                if(de < m_higest_bid) {
                    // then you lose
                    de = - m_higest_bid;
                    rr->m_what_was_the_bid = de;
                }

            }
            m_game_north_south_points += vi;
            m_game_east_west_points += de;
            rr->m_points_north_south = m_game_north_south_points;
            rr->m_points_east_west = m_game_east_west_points;

            // Inform
            post_event(Event::ROUND_OVER);
            if(!m_bContinue_game){return;}

            // ==========================================================================
            // DONE - AFTERMATH - is the game over?
            // ==========================================================================

            if (m_game_north_south_points > 61 || m_game_east_west_points >61) {
                // Who wins?
                if (m_game_north_south_points > 61 && m_game_east_west_points > 61) {
                    // Now both reached 62..
                    // The bidder wins
                    if (m_who_has_higest_bid == 0|| m_who_has_higest_bid ==2) // North South
                    {
                        m_bWinner_ns = true;
                    }
                    else {
                        m_bWinner_ew= true;
                    }

                }
                if (m_game_north_south_points > 61) {m_bWinner_ns = true;}
                if (m_game_east_west_points > 61) {m_bWinner_ew = true;}

                b_game_over = true;
                // Inform
                post_event(Event::GAME_OVER);
                if(!m_bContinue_game){return;}
            }

        }  //  end of while(!b_game_over)

    }  //  end of while() the complete game
}

int Game::check_if_player_has_cards_to_play(int who, int color){
    list<Card *>::iterator it;
    int how_many =0;

    for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
        if ((*it)->card_ranking(color) != 0 ){
            how_many++;
        }
    }
    return how_many;
}








void Game::throw_worthless_cards_on_the_table_do_not_keep_more_than_six(int who, int color, bool adjust_hand_to_six){
    list<Card *>::iterator it;
    list<Card *> tmp_list_of_cards;


    // Check the tmp list is empty
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

    for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
        if ((*it)->card_ranking(color) == 0 ) {
            //Card is worthless in this suit. throw on table
            tmp_list_of_cards.push_back(*it);      // add to the list to be informed
            m_cards_on_table[who].push_back(*it);    // put it on the table
            it = m_cards_in_hands[who].erase(it);    // take it away from hand
            it--;                                  // go back one step
        }
    }

    if (adjust_hand_to_six && m_cards_in_hands[who].size()<6){  // the last player  will not throw all cards
        while (m_cards_in_hands[who].size()<6){
            m_cards_in_hands[who].push_back(m_cards_on_table[who].front());
            m_cards_on_table[who].pop_front();
        }
    }

    //Inform
    post_event(Event::PUT_CARDS_ON_TABLE, who,tmp_list_of_cards);
    if(!m_bContinue_game){return;}

    // Empty the tmp lists afterwards
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

    // Check if a hand has now more than 6 cards
    if (m_cards_in_hands[who].size() > 6) {
        // this hand has more cards than 6
        // viible to all
        m_how_many_cards_do_i_have[who]=m_cards_in_hands[who].size();
        m_how_many_visible_cards_has_player[who]=m_cards_in_hands[who].size();
        m_how_many_visible_cards_before_buy[who]=m_cards_in_hands[who].size();

        int i_cards_to_remove = m_cards_in_hands[who].size() - 6;
        int i_rank_to_remove = 2;

        while  (i_cards_to_remove > 0)
        {
            for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
                if ((*it)->card_ranking(color) ==  i_rank_to_remove)
                {
                    tmp_list_of_cards.push_back(*it);       // Put card to be discarded into a separate list
                    m_cards_discarded[who].push_back(*it);    // Add to discarded cards

                    // need to record this card as played
                    register_card_as_played(*it, who, 0);   // as played in round 0

                    it = m_cards_in_hands[who].erase(it);     // Remove this card from the hand
                    i_cards_to_remove--;
                }
            }
            i_rank_to_remove++;
            if ( i_rank_to_remove == 4){ i_rank_to_remove=6; };
            if ( i_rank_to_remove == 10){ i_rank_to_remove=12;};

            if ( i_rank_to_remove > 15)
            {
                LOG_E("GAME - ERROR in removing cards > 6");
                throw std::runtime_error("ERROR in removing cards > 6");
            }
        }

        //Inform
        post_event(Event::KILL_CARD, who,tmp_list_of_cards);
        if(!m_bContinue_game){return;}

    }   //  end of > 6 cards

    // Empty the tmp lists afterwards
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

}


int Game::authorize_player(Player * pl) const {
    int i;

    i = -1;
    for (i=0;i<4;i++){
        if (m_pGame_player[i] == pl){
            return i;
        }
    }
    LOG_E("Game::authorize_player - could not identify plyer");
    throw std::runtime_error("Game::authorize_player - could not identify plyer");
    return -1;
}

void Game::game_init(){

    // this initialises the game (also after a completed game)

    // VI DE = 0
    m_game_north_south_points = 0;
    m_game_east_west_points = 0;
    m_bWinner_ns = false;
    m_bWinner_ew = false;

    m_who_plays.set_position(0);    // proforma

    // Game History delete
     while (!m_game_round_records.empty()) {
         RoundRecord* pRecord = m_game_round_records.front();
         m_game_round_records.pop_front();
         delete pRecord;
     }

}

void Game::round_init(){
    // this initialises the round
    // round vi de = 0

    m_my_deck.mix_deck();   // this empties any cards in the desk and restores mixes the deck

    // empty all cards in the hands, on the table and discareded cards
    for (int i=0; i<4; i++){
        while (!m_cards_in_hands[i].empty()) {
            m_cards_in_hands[i].pop_front();
        }
        while (!m_cards_on_table[i].empty()) {
            m_cards_on_table[i].pop_front();
        }
        while (!m_cards_discarded[i].empty()) {
            m_cards_discarded[i].pop_front();
        }

        //init bids to -1
        m_bids[i] = -1;
        m_player_is_cold[i] = false;
        m_player_wihtout_pidro[i] = false;
    }
    m_higest_bid = 0;              // The highest bid for the round is zero
    m_who_has_higest_bid = -1;     // who = 0..3.  -1 = nobody
    m_selected_color = -1;        // invalid color

    m_who_has_deck++;              // Move the deck  ++
    m_who_bids = m_who_has_deck;
    m_who_bids++;
    m_who_plays.set_position(0);    // proforma

    play_init();

}

int Game::get_game_our_points(Player * pl){

    int who = authorize_player(pl);

    if (who == 0 or who == 2){  //North South
        return m_game_north_south_points;
    }
    else {
        return  m_game_east_west_points;
    }
}

int Game::get_game_their_points(Player * pl){

    int who = authorize_player(pl);

    if (who == 0 or who == 2){
        return m_game_east_west_points;
    }
    else {
        return  m_game_north_south_points;
    }
}

bool Game::get_game_winner(Player * pl){
    int who = authorize_player(pl);

    if (who == 0 or who == 2){
        return m_bWinner_ns;
    }
    else {
        return m_bWinner_ew;
    }

}

int Game::get_on_table_our_points(Player * pl) const{
    int who = authorize_player(pl);
    if (who == 0 or who == 2){
        return sum_points_on_table(true);  // north south
    }
    else {
        return sum_points_on_table(false);
    }
}

int Game::get_on_table_their_points(Player * pl) const{
    int who = authorize_player(pl);
    if (who == 0 or who == 2){
        return sum_points_on_table(false);  // north south
    }
    else {
        return sum_points_on_table(true);
    }
}

int Game::get_round_our_points(Player * pl, int including_round) const{
    int who = authorize_player(pl);
    if (who == 0 or who == 2){
        return sum_round_points(true,including_round);  // north south
    }
    else {
        return sum_round_points(false,including_round);
    }
}

int Game::get_round_their_points(Player * pl, int including_round) const{
    int who = authorize_player(pl);
    if (who == 0 or who == 2){
        return sum_round_points(false,including_round);
    }
    else {
        return sum_round_points(true,including_round);   // north south
    }
}

int Game::get_roundstat_our_points(Player * pl){
    RoundRecord * rr;
    int who = authorize_player(pl);
    rr = m_game_round_records.back();
    if (who == 0 or who == 2){
        return rr->m_points_north_south; // north south
    }
    else {
        return rr->m_points_east_west;
    }
}

int Game::get_roundstat_their_points(Player * pl){
    RoundRecord * rr;
    int who = authorize_player(pl);
    rr = m_game_round_records.back();
    if (who == 0 or who == 2){
        return rr->m_points_east_west;
    }
    else {

        return rr->m_points_north_south; // north south
    }
}

string Game::get_roundstat_name(){
    RoundRecord * rr;
    rr = m_game_round_records.back();
    return rr->m_name_of_player;
}

int Game::get_roundstat_bid(){
    RoundRecord * rr;
    rr = m_game_round_records.back();
    return rr->m_what_was_the_bid;
}

int Game::get_roundstat_who_played(Player * pl){
    RoundRecord * rr;
    rr = m_game_round_records.back();
    int who = authorize_player(pl);

   // if (who == 0 or who == 2){
    int pos = rr->m_who_played;
    if(m_turn_players_viewpoint_to_south[who]){
        // now i need to turn the position
        pos = (pos + 4 - who) % 4;
    }

    return pos;
}

int Game::get_highest_bid_value(){
    int high_bid = m_bids[0];
    for (int i=1; i<4; i++){
        if (m_bids[i]>high_bid) {
            high_bid = m_bids[i];
        }
    }
    return high_bid;
}

int Game::get_bid_value_pos_absolute(int pos){

    if (pos >= 0 and pos <=3){
        return m_bids[pos];
    }
    else {
        return -1;
    }
}

int Game::get_bid_value_pos_relative_to_me(Player * pl, int offset){
    int who = authorize_player(pl);
    if (who >=0){

        return m_bids[(who + offset) % 4 ];
    }
    return -1;
}


int Game::get_who_has_deck_pos_absolute(){
    return m_who_has_deck.get_position();
}

int Game::get_who_bids_pos_absolute(){
    return m_who_bids.get_position();
}

int Game::get_who_plays_pos_absolute(){
    return m_who_plays.get_position();
}

list<Card *>  Game::get_my_cards_in_hand(Player * pl){
    int who = authorize_player(pl);
    if (who >=0){

        return m_cards_in_hands[who];
    }
    else {
        LOG_E("GAME::get_my_cards_in_hand:  No Player Authorised");
        throw std::runtime_error("Trying to access wrong player");
    }
}

list<Card *> Game::get_cards_on_the_table(int pos){

    if(pos>=0 && pos <4){
        return m_cards_on_table[pos];
    }
    else {
        LOG_E("GAME::get_cards_on_the_table: Asking for a player other than 0..3");
        throw std::runtime_error("Trying to access wrong player");
    }

}

bool Game::do_i_have_card_of_rank(Player * pl, int rank) const {
    list<Card *>::const_iterator it;
    int who = authorize_player(pl);
    if (who >=0){
        for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
            if ((*it)->card_ranking(m_selected_color) == rank ){ // got the card
                return true;
            }
        }

    }
    else {
        LOG_E("GAME::do_i_have_card_of_ran  No Player Authorised");
        throw std::runtime_error("Trying to access wrong player");
    }
    return false;
}



bool Game::is_card_of_rank_played(int rank) const{


    for(int i =0;i <= m_played_idx ;i++) {
        if(m_pPlayed_cards[i]->card_ranking(m_selected_color) == rank) {
            return true;    // is this card already played
        }
    }
    return false;
}



int Game::get_how_many_cards_in_hand_pos_relative_to_me(Player * pl, int pos){
    int who = authorize_player(pl);
    if (who >=0){

        if(m_turn_players_viewpoint_to_south[who]== true){
            pos = (who + pos) %4;
        }
        return m_cards_in_hands[pos].size();
    }
    else {
        LOG_E("GAME::get_how_many_cards_in_hand_pos_relative_to_me:  No Player Authorised");
        throw std::runtime_error("Trying to access wrong player");
    }

}




void Game::post_event(Event et){
    Rotator r;
    r = m_who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        if (m_pGame_player[r.get_position()]->inform_event(et) == false){
            quit_game();
        }
    }
}


void Game::post_event(Event et, int abs_position){
    Rotator r;
    int pos;

    r = m_who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        pos = abs_position;
        if(m_turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }
        if (m_pGame_player[r.get_position()]->inform_event(et,pos) == false){
            quit_game();
        }
    }
}

void Game::post_event(Event et, int abs_position, int value){
    Rotator r;
    int pos;

    r = m_who_has_deck;

    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        pos = abs_position;
        if(m_turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }
        if (m_pGame_player[r.get_position()]->inform_event(et,pos,value) == false){
            quit_game();
        }
    }
}

void Game::post_event(Event et, int abs_position, list<Card *>& cards ){
    Rotator r;
    int pos;
    int how_many = cards.size();

    r = m_who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next

        pos = abs_position;
        if(m_turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }

        if (et == Event::PUT_CARDS_ON_TABLE ||
            et == Event::KILL_CARD ||
            et == Event::PLAY_CARD ||
            et == Event::PLAYER_COLD) {  //this goes to everybody
            if (m_pGame_player[r.get_position()]->inform_event(et,pos,cards)== false){
                quit_game();
            }
        }

        if (et == Event::DEAL_CARD) {
            if (abs_position == r.get_position()){  //this one can see his cards. the other get the number only
                if (m_pGame_player[r.get_position()]->inform_event(et,pos,cards)== false){
                    quit_game();
                }
            }
            else{
                if (m_pGame_player[r.get_position()]->inform_event(et,pos,how_many)== false){
                    quit_game();
                }
            }
        }
    }
}

bool Game::check_if_bid_is_valid(int bid, int highbid){
    bool  ret = true;

    if (highbid == 0) { // 0  -  or 6..14
        if (!(bid==0 || (bid>=6 && bid<=14)))
        {
            ret = false;
        }
    }

    if (highbid == 14) {  // Now only 14 or 0 is accepted
        if (!(bid==0 || bid== 14 ))
        {
            ret = false;
        }
    }

    if (highbid == 5) {  // Now 6..14 is accepted
        if (!(bid>=6 && bid<=14))
        {
            ret = false;
        }
    }

    if (highbid > 0 ) {  // Normal case - must go over or pass
        if (!(bid==0 || bid==14 ||(bid> highbid && bid<=14)))
        {
            ret = false;
       }
    }
    return ret;
}

void Game::quit_game(){
    m_bContinue_game = false;
}

// ============================================================

void Game::play_init(){
    // This is the basic structure to record played cards

    // Card * played_cards[14];
    // int  played_by_whom[14];
    // int  played_in_round[14];

    m_played_idx =-1;      // no data in this structure

//   int how_many_cards_do_i_have[4];             // only to be accessed from player
//   int how_many_visible_cards_has_player[4];    // situation after color selection
//   int how_many_visible_cards_before_buy[4];    //

    for (int i =0; i<4; i++){
        m_how_many_cards_do_i_have[i]=0;
        m_how_many_visible_cards_has_player[i]=0;
        m_how_many_visible_cards_before_buy[i]=0;
    }

   m_points_left =14;
   m_available_points_left=12;
   m_previous_absolute_higest =14;
   m_absolute_higest= 14;
   m_round_playing =1;
}

void Game::register_card_as_played(Card * c, int who, int round){

    // This routine is called whenever card is played + when cards a re discarded
    // The routine can also prepare internal variable before next one plays

    //maintain the played_cards structure
    m_played_idx++;    //  Set to next element
    m_pPlayed_cards[m_played_idx] =c;
    m_played_by_whom[m_played_idx] = who;
    m_played_in_round[m_played_idx] = round;


    //For i = 0 To 3
     //  gbAndraSer(i) = MaxOf(gbAndraSer(i), gbUtspeladeKort(i))
     //Next
    if (m_how_many_visible_cards_has_player[who] < how_many_played_cards(who)){
        m_how_many_visible_cards_has_player[who] = how_many_played_cards(who);
    }

    // UpdateWithoutPidroConstruct
    //
    // TRUE när spelaren
    // inte pidrat på abs hög eller
    // inte pidrat på partnern hög sist eller
    // inte tagit pidro säker som hög sist eller

    if (get_unplayed_pidro(m_pGame_player[who])){
        if (c->card_ranking(m_selected_color) != 4  and c->card_ranking(m_selected_color) != 5) {  //' inte pidrat sjalv
            if (get_played_card((who + 2) % 4, round) == m_absolute_higest) {
                //'partner abs hog - jag pidra inte
                m_player_wihtout_pidro[who] = true;
            }
        }
        if (get_last(m_pGame_player[who])){   //' nu ar jag sist
            if (get_played_card((who + 2) % 4, round) > get_played_card((who + 1) % 4, round) and
                    get_played_card((who + 2) % 4, round) > get_played_card((who + 3) % 4, round)){
                m_player_wihtout_pidro[who] = true;
            }
            // ' sist of safe - betyder Left,Right < 4..
            if (get_played_card((who + 1) % 4, round) < 4 and get_played_card((who + 3) % 4, round) < 4){
                 m_player_wihtout_pidro[who] = true;
            }
        }
    }
}


int Game::how_many_played_cards(int pos) const {
    int ret =0;

    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_by_whom[i]==pos)
            ret++;
    }
    return ret;
}

int Game::how_many_played_cards_all() const{
    return m_played_idx + 1;
}

int Game::get_played_card_rank_idx(int idx) const{

    return m_pPlayed_cards[idx]->card_ranking(m_selected_color);
}


void Game::play_round_completed(int round){

    m_round_playing++;

    // Set new values for points_left - Points for A and 2 are deducted
    // and available_points_left -  A and 2 do not affect this
    // the card "2" needs a sparate check since it has no points set in the card_points

    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_in_round[i]==round){
            // check these cards and deduct their points
            m_points_left -= m_pPlayed_cards[i]->m_card_points;
            m_available_points_left -= m_pPlayed_cards[i]->m_card_points;

            if (m_pPlayed_cards[i]->card_ranking(m_selected_color) == 14){  // this card was A
                m_available_points_left++;
            }
            if (m_pPlayed_cards[i]->card_ranking(m_selected_color) == 1){  // this card was 2
                m_points_left--;
            }
        }
    }
    // Now evaluate the absolute_higest;
    m_previous_absolute_higest =m_absolute_higest;
    bool bfound;

    for(int j =14;j > 0 ;--j) {   // try the cards from top down
        bfound = false;
        for(int i =0;i <= m_played_idx ;i++) {
            if(m_pPlayed_cards[i]->card_ranking(m_selected_color) == j) {
                bfound = true;    // is this card already played
            }
        }
        if (bfound == false){   // if not - this is the highest
            m_absolute_higest = j;
            return;  // we are done
        }
    }
}

int Game::sum_round_points(bool NS_or_WE,int including_round) const {
    //NS_or_WE  - true calculates the North-South points together. Otherwise West-East
    int sum_ns = 0;
    int sum_we = 0;
    int sum_tmp = 0;

    // calc all available points together up to a round - use 6 for all rounds
    for (int round =0; round <= including_round; round++) {

        sum_tmp = 0;

        // check all available data
        for(int i =0;i <= m_played_idx ;i++) {
            // sum all poinst together
            if(m_played_in_round[i] == round) {
                sum_tmp += m_pPlayed_cards[i]->m_card_points;

                // special card "2" has no points set
                if (m_pPlayed_cards[i]->card_face_value() == "2") {
                    if  (m_played_by_whom[i] == 0 || m_played_by_whom[i] == 2){
                        sum_ns++;
                    }
                    else {
                        sum_we++;
                    }
                }
            }

        }
        // now decide who the sum_tmp points belongs to
        int i_who;
        i_who = who_had_the_higest_card_in_round(round,m_selected_color);
        if (i_who ==0 || i_who == 2){
            sum_ns += sum_tmp;
        }
        else {
            sum_we += sum_tmp;
        }
    }

    if (NS_or_WE){  return sum_ns;}
    return sum_we;
}


int Game::sum_points_on_table(bool NS_or_WE) const {
    //NS_or_WE  - true calculates the North-South points together. Otherwise West-East
    int sum_ns = 0;
    int sum_we = 0;
    int sum_tmp = 0;


    // check all available data
    for(int i =0;i <= m_played_idx ;i++) {
        // sum all poinst together
        if(m_played_in_round[i] == m_round_playing) {
            sum_tmp += m_pPlayed_cards[i]->m_card_points;

            // special card "2" has no points set
            if (m_pPlayed_cards[i]->card_face_value() == "2") {
                if  (m_played_by_whom[i] == 0 || m_played_by_whom[i] == 2){
                    sum_ns++;
                }
                else {
                    sum_we++;
                }
            }
        }

    }
    // now decide who the sum_tmp points belongs to
    int i_who;
    i_who = who_had_the_higest_card_in_round(m_round_playing,m_selected_color);
    if (i_who ==0 || i_who == 2){
        sum_ns += sum_tmp;
    }
    else {
        sum_we += sum_tmp;
    }

    if (NS_or_WE){  return sum_ns;}
    return sum_we;
}


int Game::who_had_the_higest_card_in_round(int round, int color) const {
    int highest = -1;
    int retval = -1;

    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_in_round[i] == round){
            if(m_pPlayed_cards[i]->card_ranking(color) > highest){
                highest = m_pPlayed_cards[i]->card_ranking(color) ;
                retval = m_played_by_whom[i];
            }
        }
    }
    return retval;
}

int Game::get_round() const{
    return m_round_playing;
}

int Game::get_cards_remaining() const{
    return 13 - m_played_idx ;    // idx is set as -1 when no card is played. First card points to item [0]
}

int Game::get_cards_left_in_my_hand(Player * pl) const{
    list<Card *>::const_iterator it;
    int ret = 0;

    int who = authorize_player(pl);
    if (who >=0){
        for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
            if ((*it)->card_ranking(m_selected_color)> 0){ // got one card
                ret++;
            }
        }
    }
    return ret;
}

int Game::get_points_left()const{
    return m_available_points_left;
}

int Game::get_position_in_round(Player * pl)const{
    int who = authorize_player(pl);
    if (who >=0){
        if(m_who_plays.get_my_start_position() == who) return Game::FIRST;
        if(m_who_plays.get_my_start_position() == (who + 1) % 4) return Game::FOURTH;
        if(m_who_plays.get_my_start_position() == (who + 2) % 4) return Game::THIRD;
        if(m_who_plays.get_my_start_position() == (who + 3) % 4) return Game::SECOND;
    }

    LOG_E("get_position_in_round - player or position not found");
    throw std::runtime_error("get_position_in_round -  player or position not found");
}

int Game::get_who_starts_pos_absolute()const{
   return m_who_plays.get_my_start_position();
}

int Game::get_bid()const{
    return m_higest_bid;
}

int Game::get_role_bid(Player * pl)const{
    int who = authorize_player(pl);
    if (who >=0 && m_who_has_higest_bid >= 0){
        if(who == m_who_has_higest_bid) return Game::BIDDER;
        if(((who + 1) % 4) == m_who_has_higest_bid ) return Game::B_DEFENCE;
        if(((who + 2) % 4) == m_who_has_higest_bid ) return Game::BIDRESPONSE;
        if(((who + 3) % 4) == m_who_has_higest_bid ) return Game::A_DEFENCE;
    }
    return -1;
}

int Game::get_my_highest_card(Player * pl)const {
    list<Card *>::const_iterator it;
    int highcard = 0;
    int val = 0;

    int who = authorize_player(pl);
    if (who >=0){
        for (it = m_cards_in_hands[who].begin(); it!=m_cards_in_hands[who].end(); ++it) {
            if ((*it)->card_ranking(m_selected_color)> highcard){ // get the highest one in color
                highcard = (*it)->card_ranking(m_selected_color);
                val = (*it)->card_ranking(m_selected_color);   // get the string value A,K...V,v,4,3,2
            }
        }
    }
    return val;

}

int Game::get_the_absolute_highest_card()const{
   return m_absolute_higest;
}
int Game::get_the_previous_absolute_highest_card()const{
   return m_previous_absolute_higest;
}

int Game::get_the_highest_card_in_round()const{
    int highest = 0;
    int val = 0;
    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_in_round[i] == m_round_playing){
            if(m_pPlayed_cards[i]->card_ranking(m_selected_color) > highest){
                highest = m_pPlayed_cards[i]->card_ranking(m_selected_color) ;
                val = m_pPlayed_cards[i]->card_ranking(m_selected_color);
            }
        }
    }
    return val;
}

int Game::get_played_card(int who, int round) const {
    int rank = 0;

    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_in_round[i] == round && m_played_by_whom[i] == who){
            rank = m_pPlayed_cards[i]->card_ranking(m_selected_color);
        }
    }
    return rank;
}

int Game::get_card_left(Player * pl)const{
    int who = authorize_player(pl);
    return get_played_card((who+1) % 4,m_round_playing );
}

int Game::get_card_partner(Player * pl)const {
    int who = authorize_player(pl);
    return get_played_card((who+2) % 4,m_round_playing );
}

int Game::get_card_right(Player * pl)const{
    int who = authorize_player(pl);
    return get_played_card((who+3) % 4,m_round_playing );
}

int Game::get_previous_card_left(Player * pl)const{
    int who = authorize_player(pl);
    return get_played_card((who+1) % 4,m_round_playing - 1 );
}

int Game::get_previous_card_partner(Player * pl)const {
    int who = authorize_player(pl);
    return get_played_card((who+2) % 4,m_round_playing - 1 );
}

int Game::get_previous_card_right(Player * pl)const{
    int who = authorize_player(pl);
    return get_played_card((who+3) % 4,m_round_playing - 1 );
}

int Game::get_previous_card_me(Player * pl)const{
    int who = authorize_player(pl);
    return get_played_card((who) % 4,m_round_playing - 1 );
}

int Game::get_previous_card_highest_round() const {
    int rank = 0;

    for(int i =0;i <= m_played_idx ;i++) {
        if(m_played_in_round[i] == m_round_playing - 1 ){
            if (m_pPlayed_cards[i]->card_ranking(m_selected_color) > rank) {
                rank = m_pPlayed_cards[i]->card_ranking(m_selected_color);
            }
        }
    }
    return rank;
}


int Game::get_selected_color()const{
    return m_selected_color;
}

int Game::get_cards_initially_in_my_hand (Player * pl) const{
    int who = authorize_player(pl);
    return m_how_many_cards_do_i_have[who];
}

int Game::get_sum_other_cards_not_me (Player * pl) const{
    int who = authorize_player(pl);
    int sum = 0;
    for (int i = 0 ;i < 4; i++)
    {
        if (i != who) {
            sum = sum + m_how_many_visible_cards_has_player[i];
        }
    }
    return sum;
}

int Game::get_cards_before_buying_left (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 1) % 4;
    return m_how_many_visible_cards_before_buy[idx];
}
int Game::get_cards_before_buying_partner (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 2) % 4;
    return m_how_many_visible_cards_before_buy[idx];
}
int Game::get_cards_before_buying_right (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 3) % 4;
    return m_how_many_visible_cards_before_buy[idx];
}


int Game::get_cards_before_buying_them_max (Player * pl) const{
    int who = authorize_player(pl);
    int a = 0;
    int b =0;
    // CardsBeforeBuyingThemMax = storre av CardsBeforeBuyingRight och CardsBeforeBuyingLeft
    // GetCardsBeforeBuyingThemMax = MaxOf(GetCardsBeforeBuyingRight, GetCardsBeforeBuyingLeft)
    if (who >= 0) {
        a = get_cards_before_buying_right(pl);
        b = get_cards_before_buying_left(pl);
        if (a > b) {
            return a;
        }
    }
    return b;
}

bool Game::get_clean_cold_by_player(int idx){
    return m_player_is_cold[idx];
}


bool Game::get_cold_by_player(Player * pl, int idx){
    int who = authorize_player(pl);
    if (m_player_is_cold[idx]) {
        return true;
    }
    //  If GetCardsLoose = 0 Then    'det finns inga obekanta kort i kopet.
    int sum = 0;
    sum = 14 - get_cards_initially_in_my_hand(pl);
    sum = sum - get_sum_other_cards_not_me(pl);

    if (sum ==0){  // CardsLoose = 0   this player sees no unknown cards
        if (get_round() > m_how_many_visible_cards_has_player[idx] and who != idx){
            // this player has not played yet (or thrown his cards)
            // it is not me and i can calculate that he is cold

            return true;
        }
    }
    return false;
}

int Game::get_cards_unknown(Player * pl){
    // 'Da ar platserna:
    // 'SUM alla obekanta hander av:
    // '( 6- Max(synliga kort efter kop, spelade kort)) eller 0 om Cold (raknad Cold OK)
    // 'egen hand raknas inte.
    int who = authorize_player(pl);
    int sum = 0;
    int tmp = 0;

    for (int i = 0 ; i < 4; i++){
        if (i != who){    // not me
            if (get_cold_by_player(pl,i) == false) {   // if Cold then  = 0
                tmp = 6 - m_how_many_visible_cards_has_player[i]; // kompliment
                if (tmp > 0){
                    sum += tmp;
                }
            }
        }
    }
    return sum;
}

bool Game::get_unplayed_pidro(Player * pl){
//    int who = authorize_player(pl);

    if (is_card_of_rank_played(4) and is_card_of_rank_played(5) ) {
        //bada pidrona spelade
        return false;
    }
    if (is_card_of_rank_played(4) and do_i_have_card_of_rank(pl,5) ) {
        // 5 ospelad - har jag den
        return false;
    }
    if (is_card_of_rank_played(5) and do_i_have_card_of_rank(pl,4) ) {
        // 4 ospelad - har jag den
        return false;
    }
    if (do_i_have_card_of_rank(pl,5) and do_i_have_card_of_rank(pl,4) ) {
        //  i have both pidro
        return false;
    }
    return true;
}

bool Game::get_last(Player * pl){
    int who = authorize_player(pl);
    int StartsRound = get_who_starts_pos_absolute();

    // "FOURTH"
    if ((who + 1) % 4 == StartsRound) {
        return true;
    }
    // "THIRD"
    if ((who + 2) % 4 == StartsRound and get_cold_by_player(pl,(who + 1) % 4)) {
        return true;
    }
    //"SECOND"
    if ((who + 3) % 4 == StartsRound and get_cold_by_player(pl,(who + 1) % 4) and get_cold_by_player(pl,(who + 2) % 4)) {
         return true;
    }
    return false;
}

bool Game::get_without_pidro_left (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 1) % 4;
    return m_player_wihtout_pidro[idx];
}
bool Game::get_without_pidro_partner (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 2) % 4;
    return m_player_wihtout_pidro[idx];
}
bool Game::get_without_pidro_right (Player * pl) const{
    int who = authorize_player(pl);
    int idx = (who + 3) % 4;
    return m_player_wihtout_pidro[idx];
}


float Game::get_expected_one_person (Player * pl, int offset){
    int who = authorize_player(pl);
    int idx;
    float visible;
    int loose = 0;   // loose cards

    loose = 14 - get_cards_initially_in_my_hand(pl);
    loose = loose - get_sum_other_cards_not_me(pl);

    idx = (who + offset) % 4;

    if (who == idx){  //myself
        visible = m_how_many_cards_do_i_have[who];
    }
    else {    // others
        visible = m_how_many_visible_cards_has_player[idx];
    }
    //   ' om en person ar kall sa ar saken klar - ingen dyn exp.
    if (get_cold_by_player(pl, idx) )
    {
        return visible;
    }

    return visible + ((6 - visible) * loose / get_cards_unknown(pl));

}


// ============================================================


void Game::print(ostream& o) const {
    o << endl;

    /*
    list<Card *>::const_iterator it;



    o << "The Deck left:" << endl;
    o << my_deck;
    for (int i = 0; i<4; i++){

        o <<"Hand (" << i << "):   ";
        for (it = cards_in_hands[i].begin(); it!=cards_in_hands[i].end(); ++it) {
            o << **it ;
        }
        o << endl ;
    }
    */

    // o << "Points North-South " << sum_round_points(true,6)<<" Points East-west " << sum_round_points(false,6) << endl;

    /*
    for (int i = 0; i<4; i++){
        o << i << ": how_many_cards_do_i_have " <<  " \t\t " <<  how_many_cards_do_i_have[i]<< endl;
        o << i<< ": how_many_visible_cards_has_player "  << " \t " <<  how_many_visible_cards_has_player[i]<< endl;
        o << i<< ": how_many_visible_cards_before_buy "  << " \t " <<  how_many_visible_cards_before_buy[i]<< endl;
        o << endl;
    }
    */


//    o << "points_left "  << " \t\t " <<  points_left<< endl;
//    o << "get_points_left() "  << " \t " <<  get_points_left()<< endl;
    o << "absolute_higest "  << " \t " <<  m_absolute_higest<< endl;
    o << "get_the_absolute_highest_card() "  << " \t " <<  get_the_absolute_highest_card()<< endl;
    o << "get_the_highest_card_in_round() "  << " \t " <<  get_the_highest_card_in_round()<< endl;


    o << endl;


    o << "get_round() "  << " \t\t " <<  get_round() << endl;
    o << "get_cards_remaining() "  << " \t " <<  get_cards_remaining() << endl;
    o << endl;

/*
    for (int i = 0; i<4; i++){
        o <<  i << ": get_cards_left_in_my_hand " << get_cards_left_in_my_hand(game_player[i]) << endl;
    }

*/

}

}
