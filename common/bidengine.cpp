#include <pidro/bidengine.h>
#include <cassert>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <pidro/deck_card.h>
#include <pidro/game.h>
#include <pidro/player.h>

extern "C" {
#include "luah/lua.h"        //Lua main library (lua_*)
#include "luah/lauxlib.h"
#include "luah/lualib.h"
}

using namespace std;

//map<string, float> gameSettings;

namespace Pidro
{

BidEngine::BidEngine(Game *game, Player *player, string eaFile, string bidRuleFile)
    : m_theGame(game)
    , m_me(player)
    , m_my_suit(Card::SPADES)
{
    //PidroBidRulesAI_Master013v1.txt
    strcpy(m_eaFile, eaFile.c_str());
    strcpy(m_eaBidRuleFile, bidRuleFile.c_str());

    if (!readEAFile(m_eaFile))
    {
        LOG_D("rule file not correct.");
        //cout << "rule file not correct.";
        exit(-1);
    }
}
/*
BidEngine::BidEngine(string eaFile, string BidRuleFile)
{
    //PidroBidRulesAI_Master013v1.txt
    strcpy(m_eaFile, eaFile.c_str());
    strcpy(m_eaBidRuleFile, BidRuleFile.c_str());

    if (!readEAFile(m_eaFile))
    {
        LOG_D("rule file not correct.");
        //cout << "rule file not correct.";
        exit(-1);
    }
}
*/
BidEngine::~BidEngine(){
   //  lua_close(m_L);   // close lua
}


float BidEngine::getEABestValue(int bid) const {
    // We search the best EA value for the given bid
    // the ea values are as follows [EA no loss],[EA bid 6],[EA bid 7], ..., [EA bid 14]
   if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getEABestValue(int bid) with invalid bid");
    }
    // translate the bid value to the index in the structure:
    // 6 --> index: 0
    // 7 --> index: 1
    float bestEA=-200;
    for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->m_eavalues[bid - 6] > bestEA) bestEA = m_myEAs[suit]->m_eavalues[bid - 6];
    }
    return bestEA;
}

Card::Suit BidEngine::getEABestSuit(int bid) const {
    if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getEABestColor(int bid) with invalid bid");
    }

    Card::Suit bestSuit = static_cast<Card::Suit>(-1);
    float bestEA=-200;
    for (auto suit : Card::Suits) {
        if (m_myEAs[suit]->m_eavalues[bid - 6] > bestEA)
        {
            bestEA = m_myEAs[suit]->m_eavalues[bid - 6];
            bestSuit = suit;
          }
    }
    return bestSuit;
}
float BidEngine::getPLevel(int bid) const {
    // We search the best (== the highest) probability value for the given bid
    // the ea values are as follows [EA no loss],[EA bid 6],[EA bid 7], ..., [EA bid 14]
    if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getPLevel(int bid) with invalid bid");
    }
    // translate the bid value to the index in the structure:
    // 6 --> index: 0
    // 7 --> index: 1
    float bestPLevel=-200;
    for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->m_probability[bid - 6] > bestPLevel) bestPLevel = m_myEAs[suit]->m_probability[bid - 6];
    }
    return bestPLevel;
}

Card::Suit BidEngine::getPLevelSuit(int bid) const {
    if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getPLevel(int bid) with invalid bid");
    }

    float bestPLevel = -200;
    Card::Suit bestSuit = static_cast<Card::Suit>(-1);

    for (auto suit : Card::Suits) {
        if (m_myEAs[suit]->m_probability[bid - 6] > bestPLevel) {
            bestPLevel = m_myEAs[suit]->m_probability[bid - 6];
            bestSuit = suit;
        }
    }
    return bestSuit;
}


float BidEngine::getWorstEAuH(int pos) const {
    if (pos < 0 or pos > 3) {
        LOG_F("FATAL - trying to ask for getWorstEAuH with invalid position");
    }
    float worstEAuHLevel=300;
     for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->m_eaNBs[pos] < worstEAuHLevel) {
            worstEAuHLevel = m_myEAs[suit]->m_eaNBs[pos];

        }
    }
    return worstEAuHLevel;
}

bool BidEngine::readEAFile(string filename) {
    ifstream infile(filename.c_str());
    bool notEOF;
    try {
        if (!infile.is_open()) throw 1;
        string line;
        getline(infile, line);
        do
        {
            if (line.find('<') != string::npos) {
                // line contains "<" (i.e. it's a new number of cards)
                auto startpos = line.find('<');
                auto endpos = line.find('>');
                string cardsStr = line.substr(startpos + 1, endpos - startpos);
                cardcount cc;
                // check if there's a _ sign
                startpos = cardsStr.find('_');
                if (startpos == string::npos) {
                    cc.m_numOfCards = atoi(cardsStr.c_str());
                    cc.m_nonImportant = 0;
                } else {
                    cc.m_numOfCards = atoi(cardsStr.substr(0, startpos).c_str());
                    cc.m_nonImportant = atoi(cardsStr.substr(startpos + 1).c_str());
                }
                getline(infile, line);
                while (line.find('<') == string::npos) {
                    char cardsConfig1[20], cardsConfig2[20], cardsConfig3[20];
                    char s1[20],s2[20],s3[20],s4[20],s5[20],s6[20],s7[20],s8[20],s9[20],s10[20];
                    // float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig1,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);
                    oneHand oh;

// skip this one - the double EaUH
                    //oh.set_eaNB((float)atof(s1));
                    oh.set_eaValues((float)atof(s2), (float)atof(s3), (float)atof(s4), (float)atof(s5), (float)atof(s6), (float)atof(s7), (float)atof(s8), (float)atof(s9), (float)atof(s10));

                    getline(infile, line);
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig2,s1,s2,s3,s4,s5,s6,s7,s8,s9);
                    oh.set_probability((float)atof(s1), (float)atof(s2), (float)atof(s3), (float)atof(s4), (float)atof(s5), (float)atof(s6), (float)atof(s7), (float)atof(s8), (float)atof(s9));

                    getline(infile, line);
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig3,s1,s2,s3,s4);
                    oh.set_eaNBs((float)atof(s1), (float)atof(s2), (float)atof(s3), (float)atof(s4));

                    // compare the cards of all three lines, if they do not match, return false
                    if (strcmp(cardsConfig1, cardsConfig2) != 0 || strcmp(cardsConfig1, cardsConfig3) != 0) return false;

                    cc.m_cardCombos[cardsConfig1] = oh;

                    notEOF = getline(infile, line);
                    startpos = line.find('<');
                    if (startpos != string::npos || !notEOF) {
                        //cc.cardCombos[cardsConfig1] = oh;
                        break;
                    }
                }
                m_cardcounts.push_back(cc);
                //std::cin.ignore();

            }
        }
        while (notEOF);
    }
    catch (int e)
    {
        if (e==1)
            //cout << "bidnet File was not found";
            LOG_D("bidnet file was not found.");
    }
    return true;
}

float BidEngine::getEA(int cardsCount, int nonImportant, string cardsConfig, int bid) {
    cardcount *cc;
    bool found = false;
    list<cardcount>::iterator it;
    for (it = m_cardcounts.begin(); it != m_cardcounts.end(); it++)
    {
        if (it->m_numOfCards == cardsCount && it->m_nonImportant == nonImportant) {
            cc = &(*it);
            found = true;
        }
    }
    if (found) { return cc->m_cardCombos[cardsConfig].get_eavalue(bid); }

    assert(!true); // An error if we arrive here?
    return false;
}

oneHand *BidEngine::getEAstruct(int cardsCount, int nonImportant, string cardsConfig) {
    cardcount *cc;
    bool found = false;
    list<cardcount>::iterator it;
    for (it = m_cardcounts.begin(); it != m_cardcounts.end(); it++)
    {
        if (it->m_numOfCards == cardsCount && it->m_nonImportant == nonImportant) {
            cc = &(*it);
            found = true;
            break;
        }
    }
    if (found)
        return &(cc->m_cardCombos[cardsConfig]);
    else {
        assert(found == true);
        return NULL;
    }
}


float oneHand::get_eavalue(int bid) { return m_eavalues[bid - 6]; }


cardsConfig BidEngine::examineHand(list<Card *> hand)
{
    int iCounter;

    // store the cards in an array
    list<Card *> array_hand[4];

    // Store the values per color to create the search string <4_2>
    int no_of_cards[4];
    int no_of_unimportant_cards[4];

    // Store the key to look for [_A_K_] per suit
    string str_card_search[4];

    // tmp variables used to reduce a large suit down to 6 cards
    int i_cards_to_remove;
    int i_rank_to_remove;

    // loop though the colors and check cards by rank
    for (auto suit: Card::Suits)
    {
        // use this loop to reset the search keys
        no_of_cards[suit] = 0;
        no_of_unimportant_cards[suit] = 0;
        str_card_search[suit]="";

        // lua - we will pass a table
        lua_newtable(m_L);
        iCounter = 0;

        list<Card *>::iterator hand_iterator;
        for (hand_iterator=hand.begin(); hand_iterator != hand.end(); hand_iterator++)
        {

            if (((Card *)(*hand_iterator))->card_ranking(suit) > 0)
            {
                // Show the card
                //LOG_D(((Card *)(*hand_iterator))->card_suit_short() + "_" + (*hand_iterator)->card_face_value() + " ");
                //LOG_D( "  Ranking Name " + ((Card *)(*hand_iterator))->card_ranking_name(suit) + "  " );
                array_hand[suit].push_back(((Card *)(*hand_iterator)));

                // push the card into the lua script
                iCounter++;                // starting with 1
                lua_pushnumber(m_L, iCounter);   // Push the table index
                lua_pushnumber(m_L, (*hand_iterator)->card_ranking(suit) ); // Push the cell value
                lua_rawset(m_L, -3);      // Stores the pair in the table
            }
        }
        //LOG_D("");

        /* By what name is the script going to reference the card table? */
        if (suit == 0)         lua_setglobal(m_L, "Hand0" );
        if (suit == 1)         lua_setglobal(m_L, "Hand1" );
        if (suit == 2)         lua_setglobal(m_L, "Hand2" );
        if (suit == 3)         lua_setglobal(m_L, "Hand3" );
    }

    // need to check if a hand is has more than 6 cards
    // if so  - certain cards need to be omitted...
    for (auto suit : Card::Suits)
    {
        //TEST_display(array_hand[suit]);
        if (array_hand[suit].size() > 6)
        {
            LOG_D(" ... more thean 6 cards");
        }
        i_cards_to_remove = array_hand[suit].size() - 6;
        i_rank_to_remove = 2;
        while  (i_cards_to_remove > 0)
        {
            list<Card *>::iterator i = array_hand[suit].begin();
            while (i != array_hand[suit].end())
            {
                if ( ((Card *)(*i))->card_ranking(suit) == i_rank_to_remove )
                {
                    LOG_D("remove card with rank ");
                    LOG_D(i_rank_to_remove);
                    array_hand[suit].erase(i++);
                } else i++;
            }

            i_rank_to_remove++;
            if ( i_rank_to_remove == 4){ i_rank_to_remove=6; };
            if ( i_rank_to_remove == 10){ i_rank_to_remove=12;};
            if ( i_rank_to_remove > 15)
            {
                LOG_D("ERROR IN LOOP");
                i_cards_to_remove = 0;
            }
        }
    }

    // Create a <Section> and [Key] String..

    // now the suits have not more than 6 cards
    // create the search keys for each suit
    for (auto suit : Card::Suits)
    {
        no_of_cards[suit] = array_hand[suit].size();  // record the number of cards in a suit
        no_of_unimportant_cards[suit] = 0;    // init to zero

        // prepocess the cards and calculate number on unimportant cards (for hands larger or qual 4 cards)
        // remove those cards from the array since they have been considered

        list<Card *>::iterator arrayHandIterator;
        for (arrayHandIterator = array_hand[suit].begin();
             arrayHandIterator != array_hand[suit].end();
             arrayHandIterator++)
        {
            if (array_hand[suit].size() >= 4)
            {
                int itmp = ((Card *)(*arrayHandIterator))->card_ranking(suit);
                if (array_hand[suit].size() == 4)
                {
                    // The cards [9,8,7,6] is considered as M
                    if (itmp == 9 || itmp == 8 || itmp == 7 || itmp == 6)
                    {
                        no_of_unimportant_cards[suit]++;
                    }
                } else { // this is the syntax for 5 and 6 cards
                    // The cards [9,8,7,6, 4, 3] is considered as M
                    if (itmp == 9 or itmp == 8 or itmp == 7 or itmp == 6 or itmp == 3 or itmp == 2) // ranking values of 4 is 3 (and 3 is 2)
                    {
                        no_of_unimportant_cards[suit]++;
                    }
                }
            }
        }

        // Now start building the string
        str_card_search[suit] = "[_";
        for (int j = 14; j >=1 ; --j)
        {
            // Loop from 14 = A down to 2 (ranking vaue 1) to create a sorted string
            if (j == 9)
            {
                // j==9, this is the placeholder where the "M" should go (position 9)
                // if only one M - then add the M to the string
                // if more Ms the the subcategory will be different  example 5<_2>
                if ( no_of_unimportant_cards[suit] ==1)
                {
                    no_of_unimportant_cards[suit] = 0;    // one M is not considered as a separate subcategory  ex. <4_1>
                    str_card_search[suit] += "M_";      // add one M
                }
            }

            // inner loop of the cards per suit  - if a card is found it should be added
            for (list<Card *>::iterator handIterator=array_hand[suit].begin(); handIterator!=array_hand[suit].end(); handIterator++)
            {
                if (((Card *)(*handIterator))->card_ranking(suit) == j)
                {
                    // pick topmost card and analyse
                    if (array_hand[suit].size() == 4 and (j==9 || j == 8 || j==7 || j==6))
                    {
                        // do nothing - ignore these cards
                    } else {
                        if (array_hand[suit].size() > 4 and (j == 9 || j == 8 || j == 7 || j == 6 || j == 3 || j == 2))
                        {
                            // Do nothing - ignore these cards
                        }
                        else
                        {
                            // jut put the string together
                            str_card_search[suit] += ((Card *)(*handIterator))->card_ranking_name(suit);
                            str_card_search[suit] += "_";
                        }
                    }
                }
            }
        }

        str_card_search[suit] += "]";


    }
    cardsConfig ret;
    for (int suit = 0; suit < 4; suit++) {
        ret.m_number_of_cards[suit] = no_of_cards[suit];
        ret.m_number_of_unimportant_cards[suit] = no_of_unimportant_cards[suit];
        ret.m_cardsString[suit]=str_card_search[suit];
    }
    return ret;
}

int BidEngine::give_bid(int minimum){

    int result, i;
    double ea_val;
    int bid;
    const char *str;
    bool b_ea;

    bid =0;

    // Init the Lua script
    m_L = luaL_newstate();

    luaL_openlibs(m_L); /* Load Lua libraries */

    /* Load the file containing the script we are going to run */

    int status = luaL_loadfile(m_L, m_eaBidRuleFile);
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        LOG_D("Couldn't load file:");
        LOG_D(lua_tostring(m_L, -1));
        exit(-1);
    }

    //read EA values - push the cards ranking values to lua (Hand0, Hand1..Hand3)
    cardsConfig cards = examineHand(m_theGame->get_my_cards_in_hand(m_me));
    for (int suit=0; suit<4; suit++)
    {
        m_myEAs[suit]=getEAstruct(cards.m_number_of_cards[suit], cards.m_number_of_unimportant_cards[suit], cards.m_cardsString[suit]);
    }

    // push values to lua

    lua_pushinteger (m_L, m_theGame->get_game_our_points(m_me));
    lua_setglobal(m_L, "VI");

    lua_pushinteger (m_L, m_theGame->get_game_their_points(m_me));
    lua_setglobal(m_L, "DE");

    lua_pushinteger (m_L,m_theGame->get_bid_value_pos_relative_to_me(m_me, 1));
    lua_setglobal(m_L, "BidLeft");
    lua_pushinteger (m_L,m_theGame->get_bid_value_pos_relative_to_me(m_me, 2));
    lua_setglobal(m_L, "BidPartner");
    lua_pushinteger (m_L,m_theGame->get_bid_value_pos_relative_to_me(m_me, 3));
    lua_setglobal(m_L, "BidRight");

    lua_pushinteger (m_L,m_theGame->get_who_bids_pos_absolute());
    lua_setglobal(m_L, "WhoBids");

    lua_pushinteger (m_L,m_theGame->get_who_has_deck_pos_absolute());
    lua_setglobal(m_L, "WhoHasDeck");

    lua_pushinteger (m_L,m_theGame->get_highest_bid_value());
    lua_setglobal(m_L, "BidHighestRound");


    lua_newtable(m_L);
    for (i = 1; i <= 9; i++) {
        lua_pushnumber(m_L, i);
        lua_pushnumber(m_L, getEABestValue(i+5));
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "BestEAValues");

    lua_newtable(m_L);
    for (i = 1; i <= 9; i++) {
        lua_pushnumber(m_L, i);
        lua_pushnumber(m_L, getEABestSuit(i+5));
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "BestEAColor");

    lua_newtable(m_L);
    for (i = 1; i <= 9; i++) {
        lua_pushnumber(m_L, i);
        lua_pushnumber(m_L, getPLevel(i+5));
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "BestPValues");


    lua_newtable(m_L);
    for (i = 1; i <= 9; i++) {
        lua_pushnumber(m_L, i);
        lua_pushnumber(m_L, getPLevelSuit(i+5));
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "BestPColor");

    lua_newtable(m_L);
    for (i = 1; i <= 4; i++) {
        lua_pushnumber(m_L, i);
        lua_pushnumber(m_L, getWorstEAuH(i-1));    // ask for 0..3
        lua_rawset(m_L, -3);
    }
    lua_setglobal(m_L, "WorstEAuH");

    // Ask Lua to run our little script
    result = lua_pcall(m_L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(m_L, -1));
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */


    bid = lua_tonumber(m_L, -1);
  //  LOG_D("ExcecuteBid: " << bid);
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */

    assert(bid == 0 || (bid >= 6 and bid <= 14));

    ea_val = lua_tonumber(m_L, -1);
 //   LOG_D("ExecuteEAValue: " << ea_val);
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */



    str = lua_tostring(m_L, -1);
 //   LOG_D("ExcecuteTyp: " << str);

    b_ea= false;        // for BID type return
    if (strcmp(str, "EA") == 0) {
        b_ea = true;
    }
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */

    str = lua_tostring(m_L, -1);
    LOG_D("Rule Name: " << str);

    //    take the strings off the stack
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */

    //  this needs to be checked again where to open close etc...
    lua_close(m_L);   // close lua


    //post process

    //cin.get();

    if (minimum == 5) {
        // this is the signal that i must take the bid
        // Get my best color
        m_my_suit = getEABestSuit(6);
        return 6;
    }

    // consider the color to choose

    if (b_ea == false) {         //   a BID type vas requested
        // which color?
        if (bid > 0) {           //  if PASS, I cannot ask for a color
            m_my_suit = getEABestSuit(bid);
        }
    }
    else                            //   a EA type vas requested
    {
        // consider color and possible BID according to EA value
        for (i = 14; i >=6; --i){
            if(getEABestValue(i)> ea_val) {
                // the requested ea value is matched and i is the bid i propose
                bid = i;
                // which color?
                m_my_suit = getEABestSuit(bid);
                break;  //end loop
            }
        }
    }

    // Now validate the BID
    if (bid >= 6 and (bid > minimum or bid == 14)){    // valid bid request

        // if i am last no need to overdo bid
        if (m_theGame->get_who_bids_pos_absolute() == m_theGame->get_who_has_deck_pos_absolute() )
        {
            bid = minimum +1;   // just bid the next highest
            if (bid > 14 ) bid = 14;   // make sure not to exceed 14
        }
        return bid;
    }

    // not a valid BID - PASS
    return 0;
}

Card::Suit BidEngine::give_suit() const
{
    return m_my_suit;
}

}
