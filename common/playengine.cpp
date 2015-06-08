#include <pidro/playengine.h>
#include <string.h>

extern "C" {
#include "luah/lua.h"        //Lua main library (lua_*)
#include "luah/lauxlib.h"
#include "luah/lualib.h"
}

using namespace std;

namespace Pidro
{
  
PlayEngine::PlayEngine(Game *game, Player *player, string playRuleFile)
{
    m_theGame = game;
    m_me = player;
    strcpy(m_playRuleFile, playRuleFile.c_str());

}

PlayEngine::~PlayEngine()
{
   //  lua_close(m_L);   // close lua
}

Card* PlayEngine::play_card(Card::Suit suit)
 {
    PlayData pd(m_theGame,m_me);
    int result;
    const char *str_suggest;
    const char *str_name;

    // Init the Lua script
     m_L = luaL_newstate();

     luaL_openlibs(m_L); /* Load Lua libraries */

     /* Load the file containing the script we are going to run */

     int status = luaL_loadfile(m_L, m_playRuleFile);
     if (status) {
         /* If something went wrong, error message is at the top of */
         /* the stack */
         LOG_D("Couldn't load file:");
         LOG_D(lua_tostring(m_L, -1));
         exit(-1);
     }

     // push values to lua
     pd.populate_with_data(suit);
     pd.push_to_lua(m_L);



     // Ask Lua to run our little script
     result = lua_pcall(m_L, 0, LUA_MULTRET, 0);
     if (result) {
         fprintf(stderr, "Failed to run script: %s\n", lua_tostring(m_L, -1));
         exit(1);
     }


     // Get the returned value at the top of the stack (index -1)

     str_name = lua_tostring(m_L, -1);
//     LOG_D("TrackName " << str_name);
     lua_pop(m_L, 1);  // Take the returned value out of the stack

     int track_no = lua_tonumber(m_L, -1);
//     LOG_D("TrackNumber: " << track_no);
     lua_pop(m_L, 1);  // Take the returned value out of the stack

     int lua_play_card = lua_tonumber(m_L, -1);
//     LOG_D("PlayCard: " << lua_play_card);
     lua_pop(m_L, 1);  // Take the returned value out of the stack

     str_suggest = lua_tostring(m_L, -1);
//     LOG_D("Suggest: " << str_suggest);
     lua_pop(m_L, 1);  // Take the returned value out of the stack

     str_name = lua_tostring(m_L, -1);
     LOG_D("Name: ");
     cout << str_name;
     lua_pop(m_L, 1);  // Take the returned value out of the stack



     //  this needs to be checked again where to open close etc...
     lua_close(m_L);   // close lua


     //post process
     list<Card *> list_of_cards = m_theGame->get_my_cards_in_hand(m_me);
     list<Card *>::iterator it;
     for (it = list_of_cards.begin(); it!=list_of_cards.end(); ++it) {
         if ((*it)->card_ranking (suit) == lua_play_card )
         {
             return (*it);
         }
     }

     // override in case no card found..
     //assert
     LOG_D("No card found - play the first ");
     cin.get();
     return (*list_of_cards.begin());
 }


// =====================================================================


bool PlayData::EqualInRank(int a, int b) {
    int c = 0;

    if (a ==b) {
        return true;
    }
    if (a > b) {    //   ' Switch so that a is lower than b
        c = a;
        a = b;
        b = c;
    }

    //'alla kort mellan x1 och x2 finns i
    //'  - spelade rundor på bordet
    //'  - eller i egen hand
    //' (- eller bland dödade kort)
    bool b_ok;

    for (int i = a; i<=b; i++){
        b_ok = false;

        if (m_theGame->do_i_have_card_of_rank(m_me,i)){
            b_ok = true;
        }
        if (m_theGame->is_card_of_rank_played(i)){
            b_ok = true;

        }
        if (!b_ok){
            return false;
        }
    }
    return true;
}


int PlayData::GetPointsToKill() {
    bool b_har_highest = false;
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;
    int bid  = m_theGame->get_bid();
    int high_card = m_theGame->get_the_absolute_highest_card();

    if (m_theGame->get_role_bid(m_me) == Game::BIDDER or m_theGame->get_role_bid(m_me) == Game::BIDRESPONSE){
        // 'Spec PointsToKill
        // 'a) Situationen från förra sticket (de)
        // 'b) de A,2 i aktiva sticket
        // 'c) poäng spelade på abs hög de i aktiva sticket

        // '=14-Bid-a-b-c
        // 'Vid 0 har man ännu inte hålat

        a =  m_theGame->get_round_their_points(m_me,m_theGame->get_round() - 1);

        // 'calc b - examine om dom har highest
        // 'Cardleft

        if( m_theGame->get_card_left(m_me)== 14 or m_theGame->get_card_left(m_me)==1){
            b++;
        }
        if( m_theGame->get_card_left(m_me)== high_card) {
            b_har_highest =true;
        }

        // 'CardRight
        if( m_theGame->get_card_right(m_me)== 14 or m_theGame->get_card_right(m_me)==1){
            b++;
        }
        if( m_theGame->get_card_right(m_me)== high_card) {
            b_har_highest =true;
        }

        if (b_har_highest){
            //  'Cardleft
            if( m_theGame->get_card_left(m_me)== 11 or m_theGame->get_card_left(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_left(m_me)==5 or m_theGame->get_card_left(m_me)==4){
                c = c + 5;
            }
            //  'CardRight
            if( m_theGame->get_card_right(m_me)== 11 or m_theGame->get_card_right(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_right(m_me)==5 or m_theGame->get_card_right(m_me)==4){
                c = c + 5;
            }
            //  'CardPartner
            if( m_theGame->get_card_partner(m_me)== 11 or m_theGame->get_card_partner(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_partner(m_me)==5 or m_theGame->get_card_partner(m_me)==4){
                c = c + 5;
            }
        }

        // '=14-Bid-a-b-c
        // 'Vid 0 har man ännu inte hålat
        return 14 - bid - a - b - c;
    }
        else
    {
        // 'Spec PointsToHoleBid
        // 'a) Situationen från förra sticket (vi)
        // 'b) vi A,2 i aktiva sticket = Partner A,2
        // 'c) poäng spelade på abs hög vi i aktiva sticket
        // 'd) A,2 på egen hand
        // '=15-Bid-a-b-c-d
        // 'Vid 0 hålar budgivaren


        a =m_theGame->get_round_our_points(m_me,m_theGame->get_round()-1);

        if( m_theGame->get_card_partner(m_me)== 14 or m_theGame->get_card_partner(m_me)==1){
            b = 1;
        }
        if( m_theGame->get_card_partner(m_me) == high_card ){
            b_har_highest = true;
        }

        if (b_har_highest == false and m_theGame->do_i_have_card_of_rank(m_me,high_card)){
            if (EqualInRank(high_card,m_theGame->get_card_partner(m_me))){
                //  'Ar min partner hogst i rundan??
                if (m_theGame->get_card_partner(m_me) == m_theGame->get_the_highest_card_in_round() ){
                    b_har_highest = true;
                }
            }
        }

        //    'c
        if (b_har_highest){
            //  'Cardleft
            if( m_theGame->get_card_left(m_me)== 11 or m_theGame->get_card_left(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_left(m_me)==5 or m_theGame->get_card_left(m_me)==4){
                c = c + 5;
            }
            //  'CardRight
            if( m_theGame->get_card_right(m_me)== 11 or m_theGame->get_card_right(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_right(m_me)==5 or m_theGame->get_card_right(m_me)==4){
                c = c + 5;
            }
            //  'CardPartner
            if( m_theGame->get_card_partner(m_me)== 11 or m_theGame->get_card_partner(m_me)==10){
                c++;
            }
            if( m_theGame->get_card_partner(m_me)==5 or m_theGame->get_card_partner(m_me)==4){
                c = c + 5;
            }
        }

        // 'd
        if (m_theGame->do_i_have_card_of_rank(m_me,14)) {
            d++;
        }
        if (m_theGame->do_i_have_card_of_rank(m_me,1)) {
            d++;
        }


        // 'e
        if (m_theGame->do_i_have_card_of_rank(m_me,11) and high_card == 11) {
            e = 1;
        }
        if (m_theGame->do_i_have_card_of_rank(m_me,10) and high_card == 10) {
            e = 1;
        }

        // '=14-Bid-a-b-c-d-e
        // 'Vid 0 hålar budgivaren
        return 14 - bid - a - b - c - d - e;
    }


}

int PlayData::GetCardsLoose() {
   int sum = 0;

   sum = 14 - m_theGame->get_cards_initially_in_my_hand(m_me);
   sum = sum - m_theGame->get_sum_other_cards_not_me(m_me);

   return sum;
}

bool PlayData::GetNotColdFourthTurnThem(){
    if (m_theGame->get_cards_before_buying_left(m_me) > 3){
        return true;
    }
    if (m_theGame->get_cards_before_buying_right(m_me) > 3){
        return true;
    }
    return false;
}

bool PlayData::GetNotColdFifthTurnThem(){
    if (m_theGame->get_cards_before_buying_left(m_me) > 4){
        return true;
    }
    if (m_theGame->get_cards_before_buying_right(m_me) > 4){
        return true;
    }
    return false;
}

bool PlayData::GetNotColdSixthTurnThem(){
    if (m_theGame->get_cards_before_buying_left(m_me) > 5){
        return true;
    }
    if (m_theGame->get_cards_before_buying_right(m_me) > 5){
        return true;
    }
    return false;
}

bool PlayData::GetNotSafeFifthTurn(){
    if (m_theGame->get_cards_before_buying_left(m_me) + GetCardsLoose() > 4){
        return true;
    }
    if (m_theGame->get_cards_before_buying_right(m_me)+ GetCardsLoose() > 4){
        return true;
    }
    return false;
}

bool PlayData::GetNotSafeSixthTurn(){
    if (m_theGame->get_cards_before_buying_left(m_me) + GetCardsLoose() > 5){
        return true;
    }
    if (m_theGame->get_cards_before_buying_right(m_me)+ GetCardsLoose() > 5){
        return true;
    }
    return false;
}

bool PlayData::GetSafeFifthTurn(){
    if (m_theGame->get_cards_before_buying_left(m_me) + GetCardsLoose() < 5 and
       m_theGame->get_cards_before_buying_right(m_me)+ GetCardsLoose() < 5)
    {
        return true;
    }

    return false;
}

bool PlayData::GetSafeSixthTurn(){
    if (m_theGame->get_cards_before_buying_left(m_me) + GetCardsLoose() < 6 and
       m_theGame->get_cards_before_buying_right(m_me)+ GetCardsLoose() < 6)
    {
        return true;
    }

    return false;
}


bool PlayData::GetNotColdFifthTurnThemExpected(){
    if (GetCardsExpectedRight() > 4)
    {
        return true;
    }
    if (GetCardsExpectedLeft() > 4)
    {
        return true;
    }
    return false;
}

bool PlayData::GetNotColdSixthTurnThemExpected(){
    if (GetCardsExpectedRight() > 5)
    {
        return true;
    }
    if (GetCardsExpectedLeft() > 5)
    {
        return true;
    }
    return false;
}






float PlayData::GetCardsExpectedLeft(){
    return  m_theGame->get_expected_one_person(m_me, 1);
}
float PlayData::GetCardsExpectedPartner(){
    return  m_theGame->get_expected_one_person(m_me, 2);
}
float PlayData::GetCardsExpectedRight(){
    return  m_theGame->get_expected_one_person(m_me, 3);
}
float PlayData::GetCardsExpectedThem(){
    return  GetCardsExpectedLeft() + GetCardsExpectedRight();
}


float PlayData::GetCardsExpectedThemMax(){
    //   'CardsExpectedThemMax = storre av CardsExpectedRight och CardsExpectedLeft
    float a = GetCardsExpectedLeft();
    float b = GetCardsExpectedRight();
    if (a > b) {
        return a;
    }
    return b;
}

float PlayData::GetCardsExpectedUs(){
    return  m_theGame->get_cards_initially_in_my_hand(m_me) + GetCardsExpectedPartner();
}



PlayData::PlayData(Game *game, Player *player)
{
    int i;
    m_theGame = game;
    m_me = player;

    for (i=0;i<6;i++){
        m_MyCards[i]=0;
    }
    for (i=0;i<14;i++){
        m_PlayedCards[i]=0;
    }

    for (i=0;i<4;i++){
        m_ColdCalculated[i]=false;
        m_ColdClean[i]=false;
    }

}

void PlayData::populate_with_data(Card::Suit suit){
    int i;

    list<Card *> list_of_cards = m_theGame->get_my_cards_in_hand(m_me);
    list<Card *>::iterator it;
    i=0;
    for (it = list_of_cards.begin(); it!=list_of_cards.end(); ++it) {
        if ((*it)->card_ranking(suit) > 0)
        {
            m_MyCards[i] =(*it)->card_ranking (suit);
            i++;
        }
    }
    m_MyCardsNumber = i;

    m_Bid = m_theGame->get_bid();
    m_Cardleft = m_theGame->get_card_left(m_me);
    m_Cardright = m_theGame->get_card_right(m_me);
    m_Cardpartner = m_theGame->get_card_partner(m_me);
    m_Round = m_theGame->get_round();
    m_PreviousCardLeft = m_theGame->get_previous_card_left(m_me);
    m_PreviousCardPartner = m_theGame->get_previous_card_partner(m_me);
    m_PreviousCardRight = m_theGame->get_previous_card_right(m_me);
    m_PreviousCardMe= m_theGame->get_previous_card_me(m_me);
    m_PointsLeft = m_theGame->get_points_left();
    m_CardHighestAbsolute = m_theGame->get_the_absolute_highest_card();
    m_RoleBid= m_theGame->get_role_bid(m_me);

    m_WhoPlays =  m_theGame->get_who_plays_pos_absolute();
    m_StartsRound =  m_theGame->get_who_starts_pos_absolute();

    m_PlayedCardsNumber = m_theGame->how_many_played_cards_all();
    if (m_PlayedCardsNumber > 0){
        for (i = 0;i < m_PlayedCardsNumber;i++){
            m_PlayedCards[i] = m_theGame->get_played_card_rank_idx(i);
        }
    }
    m_PointsToKill = GetPointsToKill();

    m_PointsTakenUs = m_theGame->get_round_our_points(m_me,m_theGame->get_round()-1);
    m_PointsTakenThem = m_theGame->get_round_their_points(m_me,m_theGame->get_round()-1);
    m_PreviousCardHighestRound = m_theGame->get_previous_card_highest_round();
    m_PreviousCardHighestAbsolute = m_theGame->get_the_previous_absolute_highest_card();
    m_CardsInitiallyInMyHand = m_theGame->get_cards_initially_in_my_hand(m_me);
    m_CardsLoose = GetCardsLoose();

    m_CardsBeforeBuyingLeft = m_theGame->get_cards_before_buying_left(m_me);
    m_CardsBeforeBuyingPartner = m_theGame->get_cards_before_buying_partner(m_me);
    m_CardsBeforeBuyingRight = m_theGame->get_cards_before_buying_right(m_me);


    for (i =0 ;i < 4; i++) {
        m_ColdCalculated[i] = m_theGame->get_cold_by_player(m_me,i);
        m_ColdClean[i] = m_theGame->get_clean_cold_by_player(i);
    }

    m_CardsUnknown = m_theGame->get_cards_unknown(m_me);

    m_CardsBeforeBuyingThemMax = m_theGame->get_cards_before_buying_them_max(m_me);

    m_WithoutPidroLeft =  m_theGame->get_without_pidro_left(m_me);
    m_WithoutPidroRight =  m_theGame->get_without_pidro_right(m_me);
    m_WithoutPidroPartner =  m_theGame->get_without_pidro_partner(m_me);

    m_SafeFifthTurn = GetSafeFifthTurn();
    m_SafeSixthTurn = GetSafeSixthTurn();

    m_NotSafeFifthTurn = GetNotSafeFifthTurn();
    m_NotSafeSixthTurn = GetNotSafeSixthTurn();

    m_NotColdFourthTurnThem = GetNotColdFourthTurnThem();
    m_NotColdFifthTurnThem = GetNotColdFifthTurnThem();
    m_NotColdSixthTurnThem = GetNotColdSixthTurnThem();

    m_CardsExpectedPartner = GetCardsExpectedPartner();
    m_CardsExpectedLeft =  GetCardsExpectedLeft();
    m_CardsExpectedRight = GetCardsExpectedRight();;
    m_CardsExpectedUs = GetCardsExpectedUs();
    m_CardsExpectedThem = GetCardsExpectedThem();
    m_CardsExpectedThemMax = GetCardsExpectedThemMax();

    m_NotColdFifthTurnThemExpected = GetNotColdFifthTurnThemExpected();
    m_NotColdSixthTurnThemExpected = GetNotColdSixthTurnThemExpected();

}

void PlayData::push_to_lua(lua_State *m_L){
    int i;

    lua_newtable(m_L);

    i=0;
    for (i =0; i < m_MyCardsNumber; i++) {
        lua_pushnumber(m_L, i + 1);
        lua_pushnumber(m_L,  m_MyCards[i]);
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "MyCards");

    lua_pushinteger (m_L, m_Bid);
    lua_setglobal(m_L, "Bid");


    lua_pushinteger (m_L, m_Cardleft);
    lua_setglobal(m_L, "CardLeft");
    lua_pushinteger (m_L,  m_Cardright);
    lua_setglobal(m_L, "CardRight");
    lua_pushinteger (m_L,  m_Cardpartner);
    lua_setglobal(m_L, "CardPartner");


    lua_pushinteger (m_L, m_Round);
    lua_setglobal(m_L, "Round");
    lua_pushinteger (m_L,  m_PreviousCardLeft);
    lua_setglobal(m_L, "PreviousCardLeft");
    lua_pushinteger (m_L,  m_PreviousCardPartner);
    lua_setglobal(m_L, "PreviousCardPartner");
    lua_pushinteger (m_L,  m_PreviousCardRight);
    lua_setglobal(m_L, "PreviousCardRight");
    lua_pushinteger (m_L,  m_PreviousCardMe);
    lua_setglobal(m_L, "PreviousCardMe");

    lua_pushinteger (m_L,  m_PointsLeft);
    lua_setglobal(m_L, "PointsLeft");
    lua_pushinteger (m_L,  m_CardHighestAbsolute);
    lua_setglobal(m_L, "CardHighestAbsolute");

    if (m_RoleBid == Game::BIDDER){
        lua_pushstring (m_L, "BIDDER" );
    }
    if (m_RoleBid == Game::A_DEFENCE){
        lua_pushstring (m_L, "A_DEFENCE" );
    }
    if (m_RoleBid == Game::BIDRESPONSE){
        lua_pushstring (m_L, "BIDRESPONSE" );
    }
    if (m_RoleBid == Game::B_DEFENCE){
        lua_pushstring (m_L, "B_DEFENCE" );
    }
    lua_setglobal(m_L, "RoleBid");

    lua_pushinteger (m_L,  m_WhoPlays);
    lua_setglobal(m_L, "WhoPlays");
    lua_pushinteger (m_L,  m_StartsRound);
    lua_setglobal(m_L, "StartsRound");

    lua_newtable(m_L);
    if (m_PlayedCardsNumber > 0) {
        for (i =0; i < m_PlayedCardsNumber; i++) {
            lua_pushnumber(m_L, i + 1 );
            lua_pushnumber(m_L,  m_PlayedCards[i]);
            lua_rawset(m_L, -3);
        }
    }
    lua_setglobal(m_L, "PlayedCards");

    lua_pushinteger (m_L,  m_PointsToKill);
    lua_setglobal(m_L, "PointsToKill");
    lua_pushinteger (m_L,  m_PointsTakenUs);
    lua_setglobal(m_L, "PointsTakenUs");
    lua_pushinteger (m_L,  m_PointsTakenThem);
    lua_setglobal(m_L, "PointsTakenThem");
    lua_pushinteger (m_L,  m_PreviousCardHighestRound);
    lua_setglobal(m_L, "PreviousCardHighestRound");
    lua_pushinteger (m_L,  m_PreviousCardHighestAbsolute);
    lua_setglobal(m_L, "PreviousCardHighestAbsolute");
    lua_pushinteger (m_L,  m_CardsInitiallyInMyHand);
    lua_setglobal(m_L, "CardsInitiallyInMyHand");
    lua_pushinteger (m_L,  m_CardsLoose);
    lua_setglobal(m_L, "CardsLoose");

    lua_pushinteger (m_L,  m_CardsBeforeBuyingLeft);
    lua_setglobal(m_L, "CardsBeforeBuyingLeft");
    lua_pushinteger (m_L,  m_CardsBeforeBuyingPartner);
    lua_setglobal(m_L, "CardsBeforeBuyingPartner");
    lua_pushinteger (m_L,  m_CardsBeforeBuyingRight);
    lua_setglobal(m_L, "CardsBeforeBuyingRight");

    lua_newtable(m_L);
    for (i =0; i < 4; i++) {
        lua_pushnumber(m_L, i + 1);
        if ( m_ColdClean[i]) {
            lua_pushnumber(m_L, 1);
        }
        else
        {
            lua_pushnumber(m_L, 0);
        }
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "ColdClean");

    lua_newtable(m_L);
    for (i =0; i < 4; i++) {
        lua_pushnumber(m_L, i + 1);
        if ( m_ColdCalculated[i]) {
            lua_pushnumber(m_L, 1);
        }
        else
        {
            lua_pushnumber(m_L, 0);
        }
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "ColdCalculated");

    lua_pushinteger (m_L,  m_CardsUnknown);
    lua_setglobal(m_L, "CardsUnknown");

    lua_pushinteger (m_L,  m_CardsBeforeBuyingThemMax);
    lua_setglobal(m_L, "CardsBeforeBuyingThemMax");

    lua_pushboolean (m_L,  m_WithoutPidroLeft);
    lua_setglobal(m_L, "WithoutPidroLeft");
    lua_pushboolean (m_L,  m_WithoutPidroRight);
    lua_setglobal(m_L, "WithoutPidroRight");
    lua_pushboolean (m_L,  m_WithoutPidroPartner);
    lua_setglobal(m_L, "WithoutPidroPartner");

    lua_pushboolean (m_L,  m_SafeFifthTurn);
    lua_setglobal(m_L, "SafeFifthTurn");
    lua_pushboolean (m_L,  m_SafeSixthTurn);
    lua_setglobal(m_L, "SafeSixthTurn");

    lua_pushboolean (m_L,  m_NotSafeFifthTurn);
    lua_setglobal(m_L, "NotSafeFifthTurn");
    lua_pushboolean (m_L,  m_NotSafeSixthTurn);
    lua_setglobal(m_L, "NotSafeSixthTurn");

    lua_pushboolean (m_L,  m_NotColdFourthTurnThem);
    lua_setglobal(m_L, "NotColdFourthTurnThem");
    lua_pushboolean (m_L,  m_NotColdFifthTurnThem);
    lua_setglobal(m_L, "NotColdFifthTurnThem");
    lua_pushboolean (m_L,  m_NotColdSixthTurnThem);
    lua_setglobal(m_L, "NotColdSixthTurnThem");


    lua_pushnumber (m_L,  m_CardsExpectedPartner);
    lua_setglobal(m_L, "CardsExpectedPartner");
    lua_pushnumber (m_L,  m_CardsExpectedLeft);
    lua_setglobal(m_L, "CardsExpectedLeft");
    lua_pushnumber (m_L,  m_CardsExpectedRight);
    lua_setglobal(m_L, "CardsExpectedRight");

    lua_pushnumber (m_L,  m_CardsExpectedUs);
    lua_setglobal(m_L, "CardsExpectedUs");
    lua_pushnumber (m_L,  m_CardsExpectedThem);
    lua_setglobal(m_L, "CardsExpectedThem");
    lua_pushnumber (m_L,  m_CardsExpectedThemMax);
    lua_setglobal(m_L, "CardsExpectedThemMax");

    lua_pushboolean (m_L,  m_NotColdFifthTurnThemExpected);
    lua_setglobal(m_L, "NotColdFifthTurnThemExpected");
    lua_pushboolean (m_L,  m_NotColdSixthTurnThemExpected);
    lua_setglobal(m_L, "NotColdSixthTurnThemExpected");



     /*
        bool m_Cold[4];

        int m_PointsToKill = 0;
        int m_PointsTakenUs = 0;
        int m_PointsTakenThem = 0;

        int m_PreviousCardHighestRound = 0;
        int m_PreviousCardHighestAbsolute = 0;

        int m_Round;
        string m_RoleBid;
        int m_CardHighestAbsolute = 0;
        int m_PointsLeft = 0;
        int m_PreviousCardLeft = 0;
        int m_PreviousCardPartner = 0;
        int m_PreviousCardRight = 0;
        int m_PreviousCardMe = 0;
        int m_WhoPlays = 0;
        int m_StartsRound = 0;

        int m_CardsInitiallyInMyHand = 0;
        int m_CardsBeforeBuyingLeft = 0;
        int m_CardsBeforeBuyingPartner = 0;
        int m_CardsBeforeBuyingRight = 0;
        int m_CardsBeforeBuyingThemMax = 0;

        int m_CardsLoose = 0;
        int m_CardsUnknown = 0;

        float m_CardsExpectedPartner = 0;
        float m_CardsExpectedLeft = 0;
        float m_CardsExpectedRight = 0;
        float m_CardsExpectedUs = 0;
        float m_CardsExpectedThem = 0;
        float m_CardsExpectedThemMax = 0;

        bool m_WithoutPidroLeft = false;
        bool m_WithoutPidroRight = false;
        bool m_WithoutPidroPartner = false;

        bool m_SafeFifthTurn = false;
        bool m_SafeSixthTurn = false;
        bool m_NotSafeFifthTurn = false;
        bool m_NotSafeSixthTurn = false;
        bool m_NotColdFourthTurnThem = false;
        bool m_NotColdFifthTurnThem = false;
        bool m_NotColdSixthTurnThem = false;

        bool m_NotColdFifthTurnThemExpected = false;
        bool m_NotColdSixthTurnThemExpected = false;


    */
     //   cin.get();
}

}

