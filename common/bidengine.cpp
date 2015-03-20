#include <cassert>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cstring>


#include "bidengine.h"
#include "game.h"
#include "player.h"
#include "deck_card.h"

extern "C" {
#include "luah/lua.h"        //Lua main library (lua_*)
#include "luah/lauxlib.h"
#include "luah/lualib.h"
}


map<string, float> gameSettings;



BidEngine::BidEngine(Game *game, Player *player, string rulefile, string bidRuleFile) : BidEngine(rulefile,bidRuleFile) {
    theGame=game;
    me=player;
    m_my_color = 0;
}

BidEngine::BidEngine(string rulefile, string BidRuleFile) {
    //PidroBidRulesAI_Master013v1.txt
    strcpy(m_eaRuleFile, rulefile.c_str());
    strcpy(m_eaBidRuleFile, BidRuleFile.c_str());

    if (!readEAFile(m_eaRuleFile))
    {
        LOG_D("rule file not correct.");
        //cout << "rule file not correct.";
        exit(-1);
    }
}

BidEngine::~BidEngine(){
   //  lua_close(m_L);   // close lua
}



string BidEngine::bool_to_string(bool b){
    if(b) {
        return "\t\t\t\t#TRUE";
    }
    else {
        return "\t\t\t\t#FALSE";
    }
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
        if (m_myEAs[suit]->eavalues[bid - 6] > bestEA) bestEA = m_myEAs[suit]->eavalues[bid - 6];
    }
    return bestEA;
}

int BidEngine::getEABestColor(int bid) const {
   if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getEABestColor(int bid) with invalid bid");
    }
    int bestColor=-1;
    float bestEA=-200;
    for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->eavalues[bid - 6] >bestEA)
        {
            bestEA = m_myEAs[suit]->eavalues[bid - 6];
            bestColor = suit;
          }
    }
    return bestColor;
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
        if (m_myEAs[suit]->probability[bid - 6] > bestPLevel) bestPLevel = m_myEAs[suit]->probability[bid - 6];
    }
    return bestPLevel;
}

int BidEngine::getPLevelColor(int bid) const {
    if (bid < 6 or bid > 14) {
        LOG_F("FATAL - trying to ask for getPLevel(int bid) with invalid bid");
    }

    float bestPLevel=-200;
    int bestColor=-1;
    for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->probability[bid - 6] > bestPLevel) {
            bestPLevel = m_myEAs[suit]->probability[bid - 6];
            bestColor = suit;
        }
    }
    return bestColor;
}


float BidEngine::getWorstEAuH(int pos) const {
    if (pos < 0 or pos > 3) {
        LOG_F("FATAL - trying to ask for getWorstEAuH with invalid position");
    }
    float worstEAuHLevel=300;
     for (int suit=0; suit < 4; suit++) {
        if (m_myEAs[suit]->eaNBs[pos] < worstEAuHLevel) {
            worstEAuHLevel = m_myEAs[suit]->eaNBs[pos];

        }
    }
    return worstEAuHLevel;
}

bool BidEngine::readBidRuleFile(string filename) {
    bidrulesfile.open(filename.c_str());
    bool notEOF;
    try
    {
        if (!bidrulesfile.is_open()) throw 1;
        string line;
        getline (bidrulesfile, line);
        do {
            // if the line is "RULE", we start rule parsing
            if (_stricmp(line.c_str(), "RULE") == 0)
                ruleProcessing(bidrulesfile);
            else {
                if (_stricmp(line.c_str(), "STOP") == 0)
                {
                    return true;
                } else {

                    auto c1 = line.find(',', 0); // find occurence of first comma
                    if (c1 == string::npos) goto readnext; // if there's no comma in the line, skip to the next line
                    auto c2 = line.find(',', c1 + 1); // find occurence of second comma
                    if (c2 == string::npos) goto readnext; // if there's no second comma in the line, skip to the next line
                    {
                        string s1 = line.substr(0, c1);
                        string s2 = line.substr(c1 + 1, c2 - c1 - 1);
                        string s3 = line.substr(c2 + 1);

                        //if (_stricmp(s1.c_str(), "set")==0) settings[s2] = s3;
                        if (_stricmp(s1.c_str(), "set")==0) gameSettings[s2] = atof(s3.c_str());
                    }
                }
            }
readnext:
            notEOF = (bool)getline(bidrulesfile, line);
        } while (notEOF);
    }
    catch (int e)
    {
        if (e==1) //cout << "bid rules File was not found";
            LOG_D("bid rules file was not found.");
    }

    //assert(!true); // An error if we arrive here?
    return false;
}

void BidEngine::updateGameSettings()
{


    gameSettings["VI"] = theGame->get_game_our_points(me);
    gameSettings["DE"] = theGame->get_game_their_points(me);
    gameSettings["Ledning"] = gameSettings["VI"] - gameSettings["DE"];
    gameSettings["harPacko"] = theGame->get_who_has_deck_pos_absolute();
    gameSettings["attBjuda"] = theGame->get_who_bids_pos_absolute();
    gameSettings["BidHighestRound"] = theGame->get_highest_bid_value();
    gameSettings["BidNext"] = gameSettings["BidHighestRound"] + 1; // Next bid must be at least one higher than current
    if (gameSettings["BidNext"] > 14) gameSettings["BidNext"] = 14; // ... but limit to 14 (upper limit)
    if (gameSettings["BidNext"] <  6) gameSettings["BidNext"] =  6; // ... and to 6 (lower limit)
    gameSettings["BidPartner"] = getBidPartner();
    gameSettings["RoleInRound"] = getBidRoleInRound();
}

void BidEngine::ruleProcessing(ifstream& infile) {
    LOG_D("RULE PROCESSING STARTS");
    //cout << "RULE PROCESSING STARTS";
    rule thisRule;
    bool notEOF;
    bool inExecute = false;
    // copy all settings to this rule's settings
    /* map<std::string, string>::iterator it;
        for (it = settings.begin(); it != settings.end(); it++) {
            settingItem item;
            item.type=settingItem::SETTING_IMMEDIATE;
            item.value = it->second;
            thisRule.ruleSettings[it->first] = item;
        }*/
    // RULE keyword already found, now reading the following lines until the END keyword
    string line;
    getline (infile, line);
    do {
        if (_stricmp(line.c_str(), "end") == 0) {
            rules.push_back(thisRule);
            return;
        }
        auto c1 = line.find(',', 0); // find occurrence of first comma
        if (c1 == string::npos) // if there's no comma in the line, only accept the EXECUTES keyword
        {
            if (_stricmp(line.c_str(), "executes") == 0) {
                LOG_D("EXECUTES found.");
                //cout << "EXECUTES found" << endl;
                inExecute = true;
            }

        } else {
            // we have at least one comma in the line
            auto c2 = line.find(',', c1 + 1); // search for second comma
            if (c2 == string::npos) { // if there is only one comma in the line, only accept Name and Probability
                string s1 = line.substr(0, c1);
                string s2 = line.substr(c1 + 1);
                if (_stricmp(s1.c_str(), "name") == 0) {
                    rule::settingItem item;
                    // I expect this value to be given directly (not looked up)
                    item.value = s2;
                    item.type = BidEngine::SETTING_IMMEDIATE;
                    thisRule.ruleSettings[s1] = item;
                } else
                    /*if (_stricmp(s1.c_str(), "probability") == 0)*/ {
                    // check if the value can be converted to float
                    //rule::settingItem item;
                    //item.value = s2;
                    //if (atoi(s2.c_str())>0 || isdigit(s2.c_str()[0]) ) {
                    //	item.type = EA::SETTING_IMMEDIATE;
                    //} else {
                    //	item.type = EA::SETTING_LOOKUP;
                    //}

                    //thisRule.ruleSettings[s1] = item;
                    rule::ruleLine rl;
                    rl.valueToCheck = s1;
                    rl.currentValue = s2;
                    rl.isFunction = false;

                    if (atoi(s2.c_str())>0 || isdigit(s2.c_str()[0]) ) {
                        rl.type = BidEngine::SETTING_IMMEDIATE;
                    } else {
                        rl.type = BidEngine::SETTING_LOOKUP;
                    }
                    rl.iOperator = rule::ruleLine::SET;
                    thisRule.ruleLines.push_back(rl);

                }
            } else {
                // we have at least two commas in the line
                auto c3 = line.find(',', c2 + 1); // search for third comma
                if (c3 == string::npos) { // if there are only two commas in the line
                    string s1 = line.substr(0, c1);
                    string s2 = line.substr(c1 + 1, c2 - c1 - 1);
                    string s3 = line.substr(c2 + 1);

                    if (!inExecute) { // we do not have a line in the EXECUTE section
                        //construct rule object line by line with one ruleline object per line

                        // here we take care of lines like this
                        // d4,EABestColor,d2
                        // this basically means SET d4 to EABestColor(d2)
                        // if there are more statment of this kind, we can add it to the if clause
                        if (_stricmp(s2.c_str(), "eabestcolor") == 0 || _stricmp(s2.c_str(), "plevel") == 0) {
                            rule::ruleLine rl;
                            rl.isFunction = true;   // we have to execute a function before reading the value. The parameter is s3.
                            rl.valueToCheck = s1;
                            rl.iOperator = rule::ruleLine::SET;
                            rl.currentValue = s2;
                            rl.type = BidEngine::SETTING_LOOKUP; // value should be looked up in settings
                            rl.argument = s3;
                            if (atoi(s3.c_str()) > 0 || isdigit(s3.c_str()[0])) {
                                rl.argumentType = BidEngine::SETTING_IMMEDIATE;
                            } else {
                                rl.argumentType = BidEngine::SETTING_LOOKUP;
                            }
                            thisRule.ruleLines.push_back(rl);
                            break;
                        }

                        // Check for "RoleInRound" line. This has to be processed separately
                        if (_stricmp(s1.c_str(), "roleinround") == 0)
                        {
                            // RoleInRound is a special rule where we check if RoleInRound is one of the comma-separated values after the keyword
                            // the correct iOperator is ONEOF
                            rule::ruleLine rl;
                            rl.isFunction = false;
                            rl.valueToCheck = s1;
                            rl.currentValue = s2 + "," + s3;
                            rl.iOperator = rule::ruleLine::ONEOF;
                            rl.type = BidEngine::SETTING_IMMEDIATE;
                            thisRule.ruleLines.push_back(rl);
                            //continue;
                        } else {
                            cout << "standard item" << endl;
                            rule::ruleLine rl;
                            rl.isFunction = false;
                            rl.valueToCheck = s1;
                            rl.currentValue = s3;
                            rl.iOperator = 0;
                            if (atoi(s3.c_str()) != 0 || isdigit(s3.c_str()[0])) {
                                rl.type = BidEngine::SETTING_IMMEDIATE;
                            } else {
                                rl.type = BidEngine::SETTING_LOOKUP;
                            }
                            if (_stricmp(s2.c_str(), "<") == 0) {
                                rl.iOperator = rule::ruleLine::LT;
                            }
                            if (_stricmp(s2.c_str(), ">") == 0) {
                                rl.iOperator = rule::ruleLine::GT;
                            }
                            if (_stricmp(s2.c_str(), ":=") == 0) {
                                rl.iOperator = rule::ruleLine::SET;
                            }
                            if (_stricmp(s2.c_str(), "=") == 0) {
                                rl.iOperator = rule::ruleLine::SET;
                            }
                            if (_stricmp(s2.c_str(), "not") == 0) {
                                rl.iOperator = rule::ruleLine::NOT;
                            }
                            if (rl.iOperator == 0) {
                                LOG_E("UNKNOWN OPERATOR '" << s2 <<"'. Please fix (or uncomment line " << __LINE__ + 1 << " in " << __FILE__ << "!");
                                exit(-1);
                            }
                            thisRule.ruleLines.push_back(rl);
                            //continue;
                        }
                    } else {
                        // We have an EXECUTE line
                        thisRule.execute.probability = atoi(s1.c_str());
                        if (_stricmp(s2.c_str(), "bid") == 0) thisRule.execute.action = BidEngine::bidBID;
                        if (_stricmp(s2.c_str(), "ea") == 0) thisRule.execute.action = BidEngine::bidEA;
                        thisRule.execute.value = s3.c_str();

                    }
                } else {
                    // we have at least three commas in the line
                    auto c4 = line.find(',', c3 + 1); // search for fourth comma
                    if (c4 == string::npos) { // if there are only three commas in the line
                        // add checks here for three comma lines
                    } else {
                        // we have (at least) four commas in the line
                        string s1 = line.substr(0, c1);
                        string s2 = line.substr(c1 + 1, c2 - c1 - 1);
                        string s3 = line.substr(c2 + 1, c3 - c2 - 1);
                        string s4 = line.substr(c3 + 1, c4 - c3 - 1);
                        string s5 = line.substr(c4 + 1);
                        if (_stricmp(s2.c_str(), ":-") == 0) {
                            // make calculations
                            rule::ruleLine rl;
                            rl.isFunction = false;
                            rl.valueToCheck = s1;
                            rl.currentValue = s3;
                            rl.argument = s5;
                            if (_stricmp(s4.c_str(), "+") == 0) rl.iOperator = rule::ruleLine::CALC_ADD;
                            if (_stricmp(s4.c_str(), "-") == 0) rl.iOperator = rule::ruleLine::CALC_SUB;

                            if (atoi(s3.c_str()) > 0 || isdigit(s3.c_str()[0])) {
                                rl.type = BidEngine::SETTING_IMMEDIATE;
                            } else {
                                rl.type = BidEngine::SETTING_LOOKUP;
                            }
                            if (atoi(s5.c_str()) > 0 || isdigit(s5.c_str()[0])) {
                                rl.argumentType = BidEngine::SETTING_IMMEDIATE;
                            } else {
                                rl.argumentType = BidEngine::SETTING_LOOKUP;
                            }


                            //rl.type = EA::SETTING_IMMEDIATE;
                            thisRule.ruleLines.push_back(rl);
                        }
                    }
                }
            }
        }
        notEOF = getline (infile, line);
    } while (notEOF);
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
                    cc.numOfCards = atoi(cardsStr.c_str());
                    cc.nonImportant = 0;
                } else {
                    cc.numOfCards = atoi(cardsStr.substr(0, startpos).c_str());
                    cc.nonImportant = atoi(cardsStr.substr(startpos + 1).c_str());
                }
                getline(infile, line);
                while (line.find('<') == string::npos) {
                    char cardsConfig1[20], cardsConfig2[20], cardsConfig3[20];
                    char s1[20],s2[20],s3[20],s4[20],s5[20],s6[20],s7[20],s8[20],s9[20],s10[20];
                    // float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig1,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);
                    oneHand oh;
                    oh.set_eaNB((float)atof(s1));
                    oh.set_eaValues((float)atof(s2), (float)atof(s3), (float)atof(s4), (float)atof(s5), (float)atof(s6), (float)atof(s7), (float)atof(s8), (float)atof(s9), (float)atof(s10));

                    getline(infile, line);
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig2,s1,s2,s3,s4,s5,s6,s7,s8,s9);
                    oh.set_probability((float)atof(s1), (float)atof(s2), (float)atof(s3), (float)atof(s4), (float)atof(s5), (float)atof(s6), (float)atof(s7), (float)atof(s8), (float)atof(s9));

                    getline(infile, line);
                    sscanf(line.c_str(), "%[^,],%[^,],%[^,],%[^,],%[^,]", cardsConfig3,s1,s2,s3,s4);
                    oh.set_eaNBs((float)atof(s1), (float)atof(s2), (float)atof(s3), (float)atof(s4));

                    // compare the cards of all three lines, if they do not match, return false
                    if (strcmp(cardsConfig1, cardsConfig2) != 0 || strcmp(cardsConfig1, cardsConfig3) != 0) return false;

                    // strip [ and ]
                    //cardsConfig1++;
                    //char *key = cardsConfig1 + 1;
                    //int i = strlen(key);
                    //key[i-1] = '\0';

                    //cout << cardsConfig1+1;

                    //for (int i = 0; key[i]; i++) { key[i] = toupper(key[i]); }

                    cc.cardCombos[cardsConfig1] = oh;

                    notEOF = getline(infile, line);
                    startpos = line.find('<');
                    if (startpos != string::npos || !notEOF) {
                        //cc.cardCombos[cardsConfig1] = oh;
                        break;
                    }
                }
                cardcounts.push_back(cc);
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
    for (it = cardcounts.begin(); it != cardcounts.end(); it++)
    {
        if (it->numOfCards == cardsCount && it->nonImportant == nonImportant) {
            cc = &(*it);
            found = true;
        }
    }
    if (found) { return cc->cardCombos[cardsConfig].get_eavalue(bid); }

    assert(!true); // An error if we arrive here?
    return false;
}

oneHand *BidEngine::getEAstruct(int cardsCount, int nonImportant, string cardsConfig) {
    cardcount *cc;
    bool found = false;
    list<cardcount>::iterator it;
    for (it = cardcounts.begin(); it != cardcounts.end(); it++)
    {
        if (it->numOfCards == cardsCount && it->nonImportant == nonImportant) {
            cc = &(*it);
            found = true;
            break;
        }
    }
    if (found)
        return &(cc->cardCombos[cardsConfig]);
    else {
        assert(found == true);
        return NULL;
    }
}

//    EA::oneHand& EA::oneHand::operator =(EA::oneHand other) {
//        swap(*this, other);
//        return *this;
//    }

float oneHand::get_eavalue(int bid) { return eavalues[bid - 6]; }

int BidEngine::getBidPartner()
{
    return theGame->get_bid_value_pos_relative_to_me(me, 2);
}

int BidEngine::getBidRight()
{
    return theGame->get_bid_value_pos_relative_to_me(me, 3);
}


int BidEngine::getBidRoleInRound()
{
    //updateGameSettings();
    // make sure these two values are set and up to date
    //gameSettings["harPacko"] = theGame->get_who_has_deck_pos_absolute();
    //gameSettings["attBjuda"] = theGame->get_who_bids_pos_absolute();
    // calculate position and return it
    if ((int)(gameSettings["attBjuda"] + 0) % 4 == gameSettings["harPacko"]) return BidEngine::PL_B_BLOCK;
    if ((int)(gameSettings["attBjuda"] + 1) % 4 == gameSettings["harPacko"]) return BidEngine::PL_RESPONSE;
    if ((int)(gameSettings["attBjuda"] + 2) % 4 == gameSettings["harPacko"]) return BidEngine::PL_A_BLOCK;
    if ((int)(gameSettings["attBjuda"] + 3) % 4 == gameSettings["harPacko"]) return BidEngine::PL_LEAD;

    assert(!true);
    return -1;
}

//void EA::examineHand(QList<Card *> hand)
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
    for (int suit = 0; suit <= 3; suit++)
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
    for (int suit = 0; suit <= 3; suit++)
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
    //LOG_D("");

    // Create a <Section> and [Key] String..

    // now the suits have not more than 6 cards
    // create the search keys for each suit
    for (int suit = 0; suit <= 3; suit++)
    {
        no_of_cards[suit] = array_hand[suit].size();  // record the number of cards in a suit
        no_of_unimportant_cards[suit] = 0;    // init to zero

        // prepocess the cards and calculate number on unimportant cards (for hands larger or qual 4 cards)
        // remove those cards from the array since they have been considered

        list<Card *>::iterator arrayHandIterator;
        for (arrayHandIterator=array_hand[suit].begin();arrayHandIterator!=array_hand[suit].end();arrayHandIterator++)
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
             //   LOG_D(((Card *)(*handIterator))->card_face_value());
             //   LOG_D(((Card *)(*handIterator))->m_card_points);
             //   LOG_D(((Card *)(*handIterator))->card_ranking(suit));
             //   LOG_D(((Card *)(*handIterator))->card_ranking_name(suit));
             //   LOG_D(((Card *)(*handIterator))->card_suit());
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
        /*
        LOG_D( no_of_cards[suit]);
        LOG_D("       ");
        LOG_D( no_of_unimportant_cards[suit]);
        LOG_D("       ");
        LOG_D(str_card_search[suit]);
        */

    }
    cardsConfig ret;
    for (int suit = 0; suit < 4; suit++) {
        ret.number_of_cards[suit] = no_of_cards[suit];
        ret.number_of_unimportant_cards[suit] = no_of_unimportant_cards[suit];
        ret.cardsString[suit]=str_card_search[suit];
    }
    return ret;
}

// copied from SO
// http://stackoverflow.com/questions/236129/split-a-string-in-c
vector<string> &BidEngine::split(const string &s, char delim, vector<string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> BidEngine::split(const std::string &s, char delim) {
    vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bidHint BidEngine::getBidHint()
{

    updateGameSettings();

    //read EA values
    cardsConfig cards = examineHand(theGame->get_my_cards_in_hand(me));
    //oneHand *oh[4];
    for (int suit=0; suit<4; suit++)
    {
        //oh[suit]=getEAstruct(cards.number_of_cards[suit], cards.number_of_unimportant_cards[suit], cards.cardsString[suit]);
        m_myEAs[suit]=getEAstruct(cards.number_of_cards[suit], cards.number_of_unimportant_cards[suit], cards.cardsString[suit]);
    }


    // iterator through rules
    list<BidEngine::rule>::iterator it;
    bool line_match;
    bidHint be;
    for (it = rules.begin(); it != rules.end(); it++)
    {
        // copy all items from the gameSettings structure into the ruleSettings map
        map<string, float>::iterator settingsIt;
        for (settingsIt = gameSettings.begin(); settingsIt != gameSettings.end(); settingsIt++) {
            ostringstream ss;
            ss << (settingsIt->second);
            it->ruleSettings[settingsIt->first].value = ss.str();
        }

        LOG_BIDRULE("Rule " << it->ruleSettings["Name"].value);
        bool rule_match;
        rule_match = true;

        //check each rule for a match and then return on first match
        list<BidEngine::rule::ruleLine>::iterator line_it;
        for (line_it = it->ruleLines.begin(); line_it != it->ruleLines.end(); line_it++) {
            line_match = true;
            string value;
            float argument;

            if (line_it->argumentType == BidEngine::SETTING_IMMEDIATE)
                argument = atof(line_it->argument.c_str());
            else
                argument = atof(it->ruleSettings[line_it->argument].value.c_str());


            if (line_it->isFunction) {
                // execute function and store its result in the ruleSettings

                // set the current rule to "none" as a placeholder
                // if this value is still set after processing the functions,
                // we did not process a function for this setting. quit the app with an error message.
                it->ruleSettings[line_it->currentValue].value = "none";
                //LOG_BIDRULE("line_it->currentValue: " << line_it->currentValue);
                //LOG_BIDRULE("it->ruleSettings[line_it->currentValue].value: " << it->ruleSettings[line_it->currentValue].value);
                if (_stricmp(line_it->currentValue.c_str(), "eabestcolor") == 0) {
                    it->ruleSettings[line_it->currentValue].value = to_string(getEABestValue(argument));
                }
                if (_stricmp(line_it->currentValue.c_str(), "plevel") == 0) {
                    it->ruleSettings[line_it->currentValue].value = to_string(getPLevel(argument));
                }
                //LOG_BIDRULE("it->ruleSettings[line_it->currentValue].value: " << it->ruleSettings[line_it->currentValue].value);
                if (_stricmp(line_it->currentValue.c_str(), "none") == 0) {
                    LOG_E("No handler function available for " << line_it->currentValue);
                    exit(-1);
                }
            }

            switch (line_it->type) {
            case BidEngine::SETTING_IMMEDIATE:
                value = line_it->currentValue;
                //LOG_BIDRULE("value is " << value);
                break;
            case BidEngine::SETTING_LOOKUP:
                value = it->ruleSettings[line_it->currentValue].value;
                //LOG_BIDRULE("value is " << value);
                break;
            }


            //LOG_BIDRULE("** line_it->type:        " << line_it->type);
            //LOG_BIDRULE("** line_it->iOperator:   " << line_it->type);
            //LOG_BIDRULE("** value                 " << value);
            //LOG_BIDRULE("** it->ruleSettings[line_it->currentValue].value " << it->ruleSettings[line_it->currentValue].value);

            switch (line_it->iOperator) {

            case BidEngine::rule::ruleLine::ONEOF:
            {
                vector<string> substrings; // A vector of items that contains player positions like A_BLOCK, B_BLOCK, LEAD, RESPONSE
                vector<int> subitems; // A vector of items that contains the numerical representations of the player positions
                vector<string>::iterator substrings_it;
                vector<int>::iterator values_it;
                values_it = subitems.begin();
                substrings = split(value, ','); // split a string of the form "A_BLOCK,B_BLOCK,LEAD" into its comma-separated substrings
                // convert the substrings like "A_BLOCK" into their numerical representations in order to be able to calculate the result of the rule in the next step
                for (substrings_it=substrings.begin(); substrings_it!=substrings.end();substrings_it++)
                {
                    if (_stricmp((*substrings_it).c_str(), "a_block")==0) subitems.insert(values_it, (int)BidEngine::PL_A_BLOCK);
                    if (_stricmp((*substrings_it).c_str(), "b_block")==0) subitems.insert(values_it, (int)BidEngine::PL_B_BLOCK);
                    if (_stricmp((*substrings_it).c_str(), "lead")==0) subitems.insert(values_it, (int)BidEngine::PL_LEAD);
                    if (_stricmp((*substrings_it).c_str(), "response")==0) subitems.insert(values_it, (int)BidEngine::PL_RESPONSE);
                }
                line_match = false; // Begin with a non-match. As soon as one item matches, the whole rule matches
                for (values_it=subitems.begin();values_it!=subitems.end();values_it++)
                {
                    LOG_BIDRULE("*** Sollwert: values_it: " << *values_it);
                    LOG_BIDRULE("*** rulesett:  " << value);
                    LOG_BIDRULE("*** line_it->valueToCheck: " << line_it->valueToCheck);
                    LOG_BIDRULE("*** line_it->valueToCheck: " << it->ruleSettings[line_it->valueToCheck].value);
                    if (*values_it == atoi(it->ruleSettings[line_it->valueToCheck].value.c_str())) line_match=true;
                }
                string playerString;
                switch (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()))
                {
                case BidEngine::PL_A_BLOCK: playerString="A_BLOCK";break;
                case BidEngine::PL_B_BLOCK: playerString="B_BLOCK";break;
                case BidEngine::PL_LEAD: playerString="LEAD";break;
                case BidEngine::PL_RESPONSE: playerString="RESPONSE";break;
                }

                LOG_BIDRULE("* CHECK " << line_it->valueToCheck << " 1OF " << value << " (" << line_it->valueToCheck << " = " << playerString << "[" << it->ruleSettings[line_it->valueToCheck].value << "])" << "  " <<bool_to_string(line_match));
            }
                break;

            case BidEngine::rule::ruleLine::SET:
                it->ruleSettings[line_it->valueToCheck].value = value;
                it->ruleSettings[line_it->valueToCheck].type = BidEngine::SETTING_IMMEDIATE;
                if (_stricmp(line_it->valueToCheck.c_str(), "probability") == 0)
                {
                    int iProbabilityLevel = atoi(value.c_str());
                    int random = rand() % 101; // random number between 0 and 100
                    if (random > iProbabilityLevel) line_match = false;
                    LOG_BIDRULE("random number is " << random << ". Probability is " << iProbabilityLevel);
                }
                LOG_BIDRULE("* SET   " << line_it->valueToCheck << " to " << value << " " <<bool_to_string(line_match));
                break;

            case BidEngine::rule::ruleLine::GT:
                if (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()) <= atoi(value.c_str())) line_match = false;
                LOG_BIDRULE("* CHECK " << line_it->valueToCheck << " >  " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")" << "  " <<bool_to_string(line_match) );
                break;

            case BidEngine::rule::ruleLine::LT:
                if (atof(it->ruleSettings[line_it->valueToCheck].value.c_str()) >= atof(value.c_str())) line_match = false;
                LOG_BIDRULE("* CHECK " << line_it->valueToCheck << " <  " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")"<< "  " <<bool_to_string(line_match) );
                break;

            case BidEngine::rule::ruleLine::NOT:
                if (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()) == atoi(value.c_str())) line_match = false;
                LOG_BIDRULE("* CHECK " << line_it->valueToCheck << " <> " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")" << "  " <<bool_to_string(line_match));
                break;

            case BidEngine::rule::ruleLine::CALC_ADD:
                it->ruleSettings[line_it->valueToCheck].value = to_string(atof(value.c_str()) + argument);
                line_match = true;
                LOG_BIDRULE("* CALC " << line_it->valueToCheck << " = " << value << " + " << argument << " (" << atof(value.c_str()) + argument << ")" << "  " <<bool_to_string(line_match));
                break;

            case BidEngine::rule::ruleLine::CALC_SUB:
                it->ruleSettings[line_it->valueToCheck].value = to_string(atof(value.c_str()) - argument);
                line_match = true;
                LOG_BIDRULE("* CALC " << line_it->valueToCheck << " = " << value << " - " << argument << " (" << atof(value.c_str()) - argument << ")" << "  " <<bool_to_string(line_match));
                break;


            default:
                LOG_BIDRULE("\t\t\t\t\t\tNO OPERATOR MATCHES!");
                break;
            }
            if (!line_match) {rule_match = false;}

        }

        if (rule_match) {
            LOG_BIDRULE("Rule " << it->ruleSettings["Name"].value << " matched.");
            be = it->execute;
            break;
        } else {
            LOG_BIDRULE("Rule " << it->ruleSettings["Name"].value << " did not match.");
            LOG_BIDRULE( "-------------------------------");
        }
    }

    //bidExecute be;
    //be.action = 0;
    //be.probability = 100;
    //be.value = 10;
    return be;
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
    cardsConfig cards = examineHand(theGame->get_my_cards_in_hand(me));
    for (int suit=0; suit<4; suit++)
    {
        m_myEAs[suit]=getEAstruct(cards.number_of_cards[suit], cards.number_of_unimportant_cards[suit], cards.cardsString[suit]);
    }

    // push values to lua

    lua_pushinteger (m_L, theGame->get_game_our_points(me));
    lua_setglobal(m_L, "VI");

    lua_pushinteger (m_L, theGame->get_game_their_points(me));
    lua_setglobal(m_L, "DE");

    lua_pushinteger (m_L,theGame->get_bid_value_pos_relative_to_me(me, 1));
    lua_setglobal(m_L, "BidLeft");
    lua_pushinteger (m_L,theGame->get_bid_value_pos_relative_to_me(me, 2));
    lua_setglobal(m_L, "BidPartner");
    lua_pushinteger (m_L,theGame->get_bid_value_pos_relative_to_me(me, 3));
    lua_setglobal(m_L, "BidRight");

    lua_pushinteger (m_L,theGame->get_who_bids_pos_absolute());
    lua_setglobal(m_L, "WhoBids");

    lua_pushinteger (m_L,theGame->get_who_has_deck_pos_absolute());
    lua_setglobal(m_L, "WhoHasDeck");

    lua_pushinteger (m_L,theGame->get_highest_bid_value());
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
        lua_pushnumber(m_L, getEABestColor(i+5));
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
        lua_pushnumber(m_L, getPLevelColor(i+5));
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
    LOG_D("ExcecuteBid: " << bid);
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */

    assert(bid == 0 || (bid >= 6 and bid <= 14));

    ea_val = lua_tonumber(m_L, -1);
    LOG_D("ExecuteEAValue: " << ea_val);
    lua_pop(m_L, 1);  /* Take the returned value out of the stack */



    str = lua_tostring(m_L, -1);
    LOG_D("ExcecuteTyp: " << str);

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

    cin.get();

    if (minimum == 5) {
        // this is the signal that i must take the bid
        // Get my best color
        m_my_color = getEABestColor(6);
        return 6;
    }

    // consider the color to choose

    if (b_ea == false) {         //   a BID type vas requested
        // which color?
        if (bid > 0) {           //  if PASS, I cannot ask for a color
            m_my_color = getEABestColor(bid);
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
                m_my_color = getEABestColor(bid);
                break;  //end loop
            }
        }
    }

    // Now validate the BID
    if (bid >= 6 and (bid > minimum or bid == 14)){    // valid bid request

        // if i am last no need to overdo bid
        if (theGame->get_who_bids_pos_absolute() == theGame->get_who_has_deck_pos_absolute() )
        {
            bid = minimum +1;   // just bid the next highest
            if (bid > 14 ) bid = 14;   // make sure not to exceed 14
        }
        return bid;
    }

    // not a valid BID - PASS
    return 0;
}

int BidEngine::give_color() const{
    return m_my_color;
}
