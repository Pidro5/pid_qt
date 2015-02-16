#include <stdexcept>
#include "game.h"
#include "log.h"




Game::Game()
{
    //  Players and viewoints
    no_of_players_registered =0;


    for (int i = 0; i < 4; i++) {
        turn_players_viewpoint_to_south[i] = false;
    }

}

Game::~Game()
{

    // Game History delete
    while(!game_round_records.empty()) {game_round_records.pop_front();  }

}




void Game::register_with_game(Player * pl){
    register_with_game(pl,false);
}

void Game::register_with_game(Player * pl, bool turn_my_view_to_the_south){

    if (no_of_players_registered < 4){
        turn_players_viewpoint_to_south[no_of_players_registered] = turn_my_view_to_the_south;

        // Store the player
        game_player[no_of_players_registered] = pl;
        // Call the player to inform about registration
        game_player[no_of_players_registered]->attached_to_game(this,no_of_players_registered,turn_my_view_to_the_south);

        no_of_players_registered++;
    }
    else {
        LOG_E("Game::register_with_game - Too many players registered.");
        throw std::runtime_error("Too many players registered.");
    }

}

void Game::run_game() {
    Rotator r;
    bool b_game_over;

    if(no_of_players_registered !=4){

        // Not 4 players registered in this game - do nothing
        LOG_E("GAME: Not enough players - cannot start! ");

        return;
    }


    // Initial setting of deck position
    who_has_deck.set_position(0);
    who_has_deck--;              // Decrement to "-1" because before each round starts the deck moves ++

    b_continue_game = true;       // game should run

    while (b_continue_game){

        // ==========================================================================
        // Game init
        // ==========================================================================

        //LOG_D("GAME: Call Game Init ");
        game_init();
        post_event(Event::GAME_INIT);
        if(!b_continue_game){return;}

        b_game_over = false;     // we are still not finished with this one game

        while (!b_game_over){
            // ==========================================================================
            // Round init
            // ==========================================================================

            //LOG_D("GAME: Call Round Init ");
            round_init();
            post_event(Event::ROUND_INIT, who_has_deck.get_position());
            if(!b_continue_game){return;}


            // ==========================================================================
            // Now deal the cards 3 x 3 for each Player
            // ==========================================================================

            for (int j=0;j<3;j++){
                r = who_has_deck;
                list<Card *>::iterator it;
                list<Card *> tmp_list_of_cards;

                for (int i=0;i<4;i++){
                    r++; //deal startint wtih the next one


                    if(my_deck.deal_cards_to_hand(3,tmp_list_of_cards) != 3){
                        LOG_W("Something wrong with dealing cards.");
                        throw std::runtime_error("Something wrong with dealing cards.");
                    }
                    //tmp_list_of_cards  has 3 cards

                    //  add to hand
                    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                        cards_in_hands[r.get_position()].push_back(*it);
                    }
                    post_event(Event::DEAL_CARD, r.get_position(),tmp_list_of_cards);
                    if(!b_continue_game){return;}

                    // Empty the tmp list afterwards
                    while(!tmp_list_of_cards.empty()) {
                        tmp_list_of_cards.pop_front();

                    }
                }
            }
            // ==========================================================================
            // Now do the bidding
            // ==========================================================================
            who_bids = who_has_deck;
            who_has_higest_bid =-1;

            for (int i=0;i<4;i++){
                who_bids++;  // Start with the one after deck
                post_event(Event::ASK_FOR_BID, who_bids.get_position());
                if(!b_continue_game){return;}

                // call the players
                if (i == 3 && higest_bid == 0){    // if this is the last one and nobody bid anything
                    higest_bid = 5;
                    who_has_higest_bid = who_bids.get_position();  // the last one has the highest bid
                }

                // TRY 3 times
                // ============
                int i_try = 0;
                bool bid_ok = false;

                while (!bid_ok){
                    bids[who_bids.get_position()] = game_player[who_bids.get_position()]->give_bid(higest_bid);
                    if(!b_continue_game){return;}
                    i_try++;
                    // do some sanity check about the bid
                    bid_ok = check_if_bid_is_valid(bids[who_bids.get_position()], higest_bid);

                    if (!bid_ok && i_try>=3) {
		      LOG_W("Player "
			    << game_player[who_bids.get_position()]->get_name()
			    << " gave 3 wrong bids. Kick him!");
                        throw std::runtime_error("Player bid not allowed");
                    }
                }

                // ============
                // Bid OK - proceed
                // set new higest bid if it is higher

                if (bids[who_bids.get_position()] > higest_bid){
                    higest_bid = bids[who_bids.get_position()];
                    who_has_higest_bid = who_bids.get_position();
                }
                // if the bid was 14 - then set the later Player to having the highest bid
                if (bids[who_bids.get_position()] == 14){
                    who_has_higest_bid = who_bids.get_position();
                }

                // tell everybody about the Bid
                post_event(Event::BID_PLACED, who_bids.get_position(), bids[who_bids.get_position()]);
                if(!b_continue_game){return;}

            }  // ends the Bidding

            // ==========================================================================
            // Now inform about the winner
            // ==========================================================================

            post_event(Event::GOT_BID, who_has_higest_bid, higest_bid);
            if(!b_continue_game){return;}


            // ==========================================================================
            // Select color
            // ==========================================================================

            // TRY 3 times
            // ============
            int i_try = 0;
            bool bid_ok = false;

            while (!bid_ok){

                selected_color = game_player[who_has_higest_bid]->give_color();
                if(!b_continue_game){return;}
                i_try++;

                // do some sanity check about the color
                if (selected_color >=0 && selected_color <=3) {
                    bid_ok = true;
                }

                if (!bid_ok && i_try>=3) {
		    LOG_E("Player "
			  << game_player[who_has_higest_bid]->get_name()
			  << " gave 3 wrong colors. Kick him!");
                    throw std::runtime_error("Player color not allowed");
                }
            }

            post_event(Event::COLOR_SELECTED, who_has_higest_bid, selected_color);
            if(!b_continue_game){return;}

            // ==========================================================================
            // Hands (except the one holding the deck) throws excess cards on the table
            // ==========================================================================

            r = who_has_deck;

            for (int i=0;i<3;i++){    // Note - only 3 - until we reach the deck
                r++; //start with the next one after deck
                throw_worthless_cards_on_the_table_do_not_keep_more_than_six(r.get_position(),selected_color, false);
            }

            // ==========================================================================
            // Deal to 6 cards
            // ==========================================================================

            r = who_has_deck;

            for (int i=0;i<3;i++){    // Note - only 3 - until we reach the deck

                r++; //start with the next one after deck
                list<Card *>::iterator it;
                list<Card *> tmp_list_of_cards;

                int how_many_cards =6 - cards_in_hands[r.get_position()].size();
                if (how_many_cards > 0) {
                    if(my_deck.deal_cards_to_hand(how_many_cards,tmp_list_of_cards) != how_many_cards){
                        LOG_E("Something wrong with dealing cards.");
                        throw std::runtime_error("Something wrong with dealing cards.");
                    }
                    //  add to hand
                    for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                        cards_in_hands[r.get_position()].push_back(*it);
                    }
                    post_event(Event::DEAL_CARD, r.get_position(),tmp_list_of_cards);
                    if(!b_continue_game){return;}

                    // Empty the tmp list afterwards
                    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }
                }
            }

            // ==========================================================================
            // Deal the deck-rest to the one who has the deck.
            // ==========================================================================
            list<Card *>::iterator it;
            list<Card *> tmp_list_of_cards;
            int how_many_cards = my_deck.cards_left_in_deck();
            if (how_many_cards > 0) {
                if(my_deck.deal_cards_to_hand(how_many_cards,tmp_list_of_cards) != how_many_cards){
                    LOG_E("Something wrong with dealing cards.");
                    throw std::runtime_error("Something wrong with dealing cards.");
                }
                //  add to hand
                for (it = tmp_list_of_cards.begin(); it!=tmp_list_of_cards.end(); ++it) {
                    cards_in_hands[who_has_deck.get_position()].push_back(*it);
                }
                post_event(Event::DEAL_CARD, who_has_deck.get_position(),tmp_list_of_cards);
                if(!b_continue_game){return;}

                // Empty the tmp list afterwards
                while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }
            }

            // ==========================================================================
            // The deck throws the unnecessary cards
            // ==========================================================================

            throw_worthless_cards_on_the_table_do_not_keep_more_than_six(who_has_deck.get_position(),selected_color,true);

            // ==========================================================================
            // PLAY begins
            // ==========================================================================

            post_event(Event::BEGIN_PLAY);
            if(!b_continue_game){return;}

            // ==========================================================================
            // PLAY
            // ==========================================================================

            who_plays.set_position(who_has_higest_bid);

            for (int i_round=1;i_round<=6;i_round++){  // 6 Rounds   1..6 (Round 0 reserved for Killed cards
                bool a_card_played = false;

                for (int i_player=0;i_player<4;i_player++){  // 4 player

                    // Check if cold
                    if(!is_cold[who_plays.get_position()] && check_if_player_has_cards_to_play(who_plays.get_position(), selected_color ) == 0){
                        // this player just got cold - needs to be registered

                        is_cold[who_plays.get_position()] = true;

                        // he needs to throw his card - out of the hand - but not onto the table
                        list<Card *> tmp_list_of_cards;

                        while(!cards_in_hands[who_plays.get_position()].empty()){
                            tmp_list_of_cards.push_back(cards_in_hands[who_plays.get_position()].front());
                            cards_in_hands[who_plays.get_position()].pop_front();
                        }

                        // Inform
                        post_event(Event::PLAYER_COLD, who_plays.get_position(),
                                   cards_in_hands[who_plays.get_position()]);
                        if(!b_continue_game){return;}

                        // Empty the tmp list afterwards
                        while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }

                    }

                    if(!is_cold[who_plays.get_position()]){

                        // player has still cards - then ask for a card

                        // TRY 3 times
                        // ============
                        int i_try = 0;
                        bool play_ok = false;
                        Card * c;

                        while (!play_ok){

                            c = game_player[who_plays.get_position()]->play_card(selected_color);
                            if(!b_continue_game){return;}
                            i_try++;

                            // do some sanity check
                            if (c->card_ranking(selected_color)> 0) {  // ok card
                                play_ok = true;
                            }

                            if (!play_ok && i_try>=3) {
			        LOG_W("Player "
				      << game_player[r.get_position()]->get_name()
				      << " played 3 wrong cards. Kick him!");
                                throw std::runtime_error("Player card not allowed");
                            }
                        }
                        // if ok play it
                        register_card_as_played(c,who_plays.get_position(), i_round);
                        a_card_played=true;

                        // remove card from hand - and  put it on the table
                        cards_in_hands[who_plays.get_position()].remove(c);


                        // post card play
                        list<Card *> tmp_list_of_cards;
                        tmp_list_of_cards.push_back(c);

                        post_event(Event::PLAY_CARD, who_plays.get_position(), tmp_list_of_cards);
                        if(!b_continue_game){return;}

                        // Empty the tmp list afterwards
                        while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front();  }

                    }
                    who_plays++;   // next one in round

                }
                // Check who has the higest card in round

                if(a_card_played) {post_event(Event::PLAY_ROUND_FINISH);}
                if(!b_continue_game){return;}

                who_plays.set_position(who_had_the_higest_card_in_round(i_round, selected_color));

            }

            // ==========================================================================
            // DONE - AFTERMATH
            // ==========================================================================

            // Store the result in a list
            RoundRecord * rr = new RoundRecord();
            game_round_records.push_back(rr);
            rr->who_played = who_has_higest_bid;
            rr->what_was_the_bid = higest_bid;
            rr->name_of_player = game_player[who_has_higest_bid]->get_name();

            int vi = sum_round_points(true, 6);
            int de = sum_round_points(false, 6);
            if (who_has_higest_bid == 0|| who_has_higest_bid ==2) // North South
            {
                if(vi < higest_bid) {
                    // then you lose
                    vi = - higest_bid;
                    rr->what_was_the_bid = vi;
                }
            }
            else
            {
                if(de < higest_bid) {
                    // then you lose
                    de = - higest_bid;
                    rr->what_was_the_bid = de;
                }

            }
            game_north_south_points += vi;
            game_east_west_points += de;
            rr->points_north_south = game_north_south_points;
            rr->points_east_west = game_east_west_points;

            // Inform
            post_event(Event::ROUND_OVER);
            if(!b_continue_game){return;}

            // ==========================================================================
            // DONE - AFTERMATH - is the game over?
            // ==========================================================================

            if (game_north_south_points > 61 || game_east_west_points >61) {
                // Who wins?
                if (game_north_south_points > 61 && game_east_west_points > 61) {
                    // Now both reached 62..
                    // The bidder wins
                    if (who_has_higest_bid == 0|| who_has_higest_bid ==2) // North South
                    {
                        b_winner_ns = true;
                    }
                    else {
                        b_winner_ew= true;
                    }

                }
                if (game_north_south_points > 61) {b_winner_ns = true;}
                if (game_east_west_points > 61) {b_winner_ew = true;}

                b_game_over = true;
                // Inform
                post_event(Event::GAME_OVER);
                if(!b_continue_game){return;}
            }

        }  //  end of while(!b_game_over)

    }  //  end of while() the complete game
}

int Game::check_if_player_has_cards_to_play(int who, int color){
    list<Card *>::iterator it;
    int how_many =0;

    for (it = cards_in_hands[who].begin(); it!=cards_in_hands[who].end(); ++it) {
        if ((*it)->card_ranking(color) != 0 ){
            how_many++;
        }
    }
    return how_many;
}




void Game::register_card_as_played(Card * c, int who, int round){

    //maintain the played_cards structure
    played_idx++;    //  Set to next element
    played_cards[played_idx] =c;
    played_by_whom[played_idx] = who;
    played_in_round[played_idx] = round;
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
        for(int i =0;i <= played_idx ;i++) {
            // sum all poinst together
            if(played_in_round[i] == round) {
                sum_tmp += played_cards[i]->card_points;

                // special card "2" has no points set
                if (played_cards[i]->card_face_value() == "2") {
                    if  (played_by_whom[i] == 0 || played_by_whom[i] == 2){
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
        i_who = who_had_the_higest_card_in_round(round,selected_color);
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


int Game::who_had_the_higest_card_in_round(int round, int color) const {
    int highest = -1;
    int retval = -1;

    for(int i =0;i <= played_idx ;i++) {
        if(played_in_round[i] == round){
            if(played_cards[i]->card_ranking(color) > highest){
                highest = played_cards[i]->card_ranking(color) ;
                retval = played_by_whom[i];
            }
        }
    }
    return retval;
}




void Game::throw_worthless_cards_on_the_table_do_not_keep_more_than_six(int who, int color, bool adjust_hand_to_six){
    list<Card *>::iterator it;
    list<Card *> tmp_list_of_cards;


    // Check the tmp list is empty
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

    for (it = cards_in_hands[who].begin(); it!=cards_in_hands[who].end(); ++it) {
        if ((*it)->card_ranking(color) == 0 ) {
            //Card is worthless in this suit. throw on table
            tmp_list_of_cards.push_back(*it);      // add to the list to be informed
            cards_on_table[who].push_back(*it);    // put it on the table
            it = cards_in_hands[who].erase(it);    // take it away from hand
            it--;                                  // go back one step
        }
    }

    if (adjust_hand_to_six && cards_in_hands[who].size()<6){  // the last player  will not throw all cards
        while (cards_in_hands[who].size()<6){
            cards_in_hands[who].push_back(cards_on_table[who].front());
            cards_on_table[who].pop_front();
        }
    }

    //Inform
    post_event(Event::PUT_CARDS_ON_TABLE, who,tmp_list_of_cards);
    if(!b_continue_game){return;}

    // Empty the tmp lists afterwards
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

    // Check if a hand has now more than 6 cards
    if (cards_in_hands[who].size() > 6) {
        // this hand has more cards than 6

        int i_cards_to_remove = cards_in_hands[who].size() - 6;
        int i_rank_to_remove = 2;

        while  (i_cards_to_remove > 0)
        {
            for (it = cards_in_hands[who].begin(); it!=cards_in_hands[who].end(); ++it) {
                if ((*it)->card_ranking(color) ==  i_rank_to_remove)
                {
                    tmp_list_of_cards.push_back(*it);       // Put card to be discarded into a separate list
                    cards_discarded[who].push_back(*it);    // Add to discarded cards

                    // need to record this card as played
                    register_card_as_played(*it, who, 0);   // as played in round 0

                    it = cards_in_hands[who].erase(it);     // Remove this card from the hand
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
        if(!b_continue_game){return;}

    }   //  end of > 6 cards

    // Empty the tmp lists afterwards
    while(!tmp_list_of_cards.empty()) {tmp_list_of_cards.pop_front(); }

}


int Game::authorise_player(Player * pl) const {
    int i;

    i = -1;
    for (i=0;i<4;i++){
        if (game_player[i] == pl){
            return i;
        }
    }
    return -1;
}

void Game::game_init(){

    // this initialises the game (also after a completed game)

    // VI DE = 0
    game_north_south_points = 0;
    game_east_west_points = 0;
    b_winner_ns = false;
    b_winner_ew = false;

    who_plays.set_position(0);    // proforma

    // Game History delete
    while(!game_round_records.empty()) {game_round_records.pop_front();  }


}

void Game::round_init(){
    // this initialises the round
    // round vi de = 0

    my_deck.mix_deck();   // this empties any cards in the desk and restores mixes the deck

    // empty all cards in the hands, on the table and discareded cards
    for (int i=0; i<4; i++){
        while (!cards_in_hands[i].empty()) {
            cards_in_hands[i].pop_front();
        }
        while (!cards_on_table[i].empty()) {
            cards_on_table[i].pop_front();
        }
        while (!cards_discarded[i].empty()) {
            cards_discarded[i].pop_front();
        }

        //init bids to -1
        bids[i] = -1;
        is_cold[i] = false;
    }
    higest_bid = 0;              // The highest bid for the round is zero
    who_has_higest_bid = -1;     // who = 0..3.  -1 = nobody
    selected_color = -1;        // invalid color

    who_has_deck++;              // Move the deck  ++
    who_bids = who_has_deck;
    who_bids++;
    who_plays.set_position(0);    // proforma

    //===========
    // Card * played_cards[14];
    // int  played_by_whom[14];
    // int  played_in_round[14];
    played_idx =-1;      // no data in this structure
}

int Game::get_game_our_points(Player * pl){

    int who = authorise_player(pl);

    if (who == 0 or who == 2){  //North South
        return game_north_south_points;
    }
    else {
        if ((who != 1) || (who != 3)) {
            LOG_E("Game::get_game_our_points - could not identify plyer");
            throw std::runtime_error("Game::get_game_our_points - could not identify plyer");
        }
        return  game_east_west_points;
    }
}

int Game::get_game_their_points(Player * pl){

    int who = authorise_player(pl);

    if (who == 0 or who == 2){
        return game_east_west_points;
    }
    else {
        if ((who != 1) || (who != 3)) {
            LOG_E("Game::get_game_their_points - could not identify plyer");
            throw std::runtime_error("Game::get_game_their_points - could not identify plyer");
        }

        return  game_north_south_points;
    }
}

bool Game::get_game_winner(Player * pl){
    int who = authorise_player(pl);

    if (who == 0 or who == 2){
        return b_winner_ns;
    }
    else {
        return b_winner_ew;
    }

}

int Game::get_round_our_points(Player * pl, int including_round) const{
    int who = authorise_player(pl);
    if (who == 0 or who == 2){
        return sum_round_points(true,including_round);  // north south
    }
    else {
        return sum_round_points(false,including_round);
    }
}

int Game::get_round_their_points(Player * pl, int including_round) const{
    int who = authorise_player(pl);
    if (who == 0 or who == 2){
        return sum_round_points(false,including_round);
    }
    else {
        return sum_round_points(true,including_round);   // north south
    }
}

int Game::get_roundstat_our_points(Player * pl){
    RoundRecord * rr;
    int who = authorise_player(pl);
    rr = game_round_records.back();
    if (who == 0 or who == 2){
        return rr->points_north_south; // north south
    }
    else {
        return rr->points_east_west;
    }
}

int Game::get_roundstat_their_points(Player * pl){
    RoundRecord * rr;
    int who = authorise_player(pl);
    rr = game_round_records.back();
    if (who == 0 or who == 2){
        return rr->points_east_west;
    }
    else {

        return rr->points_north_south; // north south
    }
}

string Game::get_roundstat_name(){
    RoundRecord * rr;
    rr = game_round_records.back();
    return rr->name_of_player;
}

int Game::get_roundstat_bid(){
    RoundRecord * rr;
    rr = game_round_records.back();
    return rr->what_was_the_bid;
}

int Game::get_roundstat_who_played(Player * pl){
    RoundRecord * rr;
    rr = game_round_records.back();
    int who = authorise_player(pl);

   // if (who == 0 or who == 2){
    int pos = rr->who_played;
    if(turn_players_viewpoint_to_south[who]){
        // now i need to turn the position
        pos = (pos + 4 - who) % 4;
    }

    return pos;
}

int Game::get_highest_bid_value(){
    int high_bid = bids[0];
    for (int i=1; i<4; i++){
        if (bids[i]>high_bid) {
            high_bid = bids[i];
        }
    }
    return high_bid;
}

int Game::get_bid_value_pos_absolute(int pos){

    if (pos >= 0 and pos <=3){
        return bids[pos];
    }
    else {
        return -1;
    }
}

int Game::get_bid_value_pos_relative_to_me(Player * pl, int offset){
    int who = authorise_player(pl);
    if (who >=0){

        return bids[(who + offset) % 4 ];
    }
    return -1;
}


int Game::get_who_has_deck_pos_absolute(){
    return who_has_deck.get_position();
}

int Game::get_who_bids_pos_absolute(){
    return who_bids.get_position();
}

int Game::get_who_plays_pos_absolute(){
    return who_plays.get_position();
}

list<Card *>  Game::get_my_cards_in_hand(Player * pl){
    int who = authorise_player(pl);
    if (who >=0){

        return cards_in_hands[who];
    }
    else {
        LOG_E("GAME::get_my_cards_in_hand:  No Player Authorised");
        throw std::runtime_error("Trying to access wrong player");
    }
}

list<Card *> Game::get_cards_on_the_table(int pos){

    if(pos>=0 && pos <4){
        return cards_on_table[pos];
    }
    else {
        LOG_E("GAME::get_cards_on_the_table: Asking for a player other than 0..3");
        throw std::runtime_error("Trying to access wrong player");
    }

}

int Game::get_how_many_cards_in_hand_pos_relative_to_me(Player * pl, int pos){
    int who = authorise_player(pl);
    if (who >=0){

        if(turn_players_viewpoint_to_south[who]== true){
            pos = (who + pos) %4;
        }
        return cards_in_hands[pos].size();
    }
    else {
        LOG_E("GAME::get_how_many_cards_in_hand_pos_relative_to_me:  No Player Authorised");
        throw std::runtime_error("Trying to access wrong player");
    }

}



void Game::post_event(Event et){
    Rotator r;
    r = who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        if (game_player[r.get_position()]->inform_event(et) == false){
            quit_game();
        }
    }
}


void Game::post_event(Event et, int abs_position){
    Rotator r;
    int pos;

    r = who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        pos = abs_position;
        if(turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }
        if (game_player[r.get_position()]->inform_event(et,pos) == false){
            quit_game();
        }
    }
}

void Game::post_event(Event et, int abs_position, int value){
    Rotator r;
    int pos;

    r = who_has_deck;

    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next
        pos = abs_position;
        if(turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }
        if (game_player[r.get_position()]->inform_event(et,pos,value) == false){
            quit_game();
        }
    }
}

void Game::post_event(Event et, int abs_position, list<Card *>& cards ){
    Rotator r;
    int pos;
    int how_many = cards.size();

    r = who_has_deck;


    // message all players starting from deck + 1
    for (int i=0; i<4; i++){
        r++;  //whos next

        pos = abs_position;
        if(turn_players_viewpoint_to_south[r.get_position()]){
            // now i need to turn the position
            pos = (abs_position + 4 - r.get_position()) % 4;
        }

        if (et == Event::PUT_CARDS_ON_TABLE ||
            et == Event::KILL_CARD ||
            et == Event::PLAY_CARD ||
            et == Event::PLAYER_COLD) {  //this goes to everybody
            if (game_player[r.get_position()]->inform_event(et,pos,cards)== false){
                quit_game();
            }
        }

        if (et == Event::DEAL_CARD) {
            if (abs_position == r.get_position()){  //this one can see his cards. the other get the number only
                if (game_player[r.get_position()]->inform_event(et,pos,cards)== false){
                    quit_game();
                }
            }
            else{
                if (game_player[r.get_position()]->inform_event(et,pos,how_many)== false){
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
    b_continue_game = false;
}



void Game::print(ostream& o) const {
    list<Card *>::const_iterator it;

    o << endl;
    o << "The Deck left:" << endl;
    o << my_deck;

    for (int i = 0; i<4; i++){

        o <<"Hand (" << i << "):   ";
        for (it = cards_in_hands[i].begin(); it!=cards_in_hands[i].end(); ++it) {
            o << **it ;
        }
        o << endl ;
    }

    o << "Points North-South " << sum_round_points(true,6)<<" Points East-west " << sum_round_points(false,6) << endl;

}

