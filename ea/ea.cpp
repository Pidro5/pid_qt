#include <cassert>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

#include "ea.h"
#include "game.h"
#include "player.h"
#include "deck_card.h"

static const bool DEBUG = true;

namespace std {
    map<string, float> gameSettings;

    EA::EA() {
        strcpy(eaRuleFile, "T:\\_home\\SchachtnerTh\\RulesFiles\\Bidnet76Clean.txt");
        // if reading the rules file does not succeed, print error msg
        if (!readRuleFile(eaRuleFile)) cout << "rule file not correct.";
    }

    EA::EA(string rulefile) {
    strcpy(eaRuleFile, rulefile.c_str());
	// if reading the rules file does not succeed, print error msg
	if (!readRuleFile(eaRuleFile)) cout << "rule file not correct.";
}

    EA::EA(Game *game, Player *player, string rulefile, string bidRuleFile) : EA(rulefile,bidRuleFile) {
		theGame=game;
		me=player;
        //EA(rulefile, bidRuleFile);
	}

	EA::EA(string rulefile, string BidRuleFile) {
		//PidroBidRulesAI_Master013v1.txt
        strcpy(eaRuleFile, rulefile.c_str());
        strcpy(eaBidRuleFile, BidRuleFile.c_str());
		if (!readRuleFile(eaRuleFile)) cout << "rule file not correct.";
		if (!readBidRuleFile(eaBidRuleFile)) cout << "bid rule file  not correct.";
	}

	bool EA::readBidRuleFile(string filename) {
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
                    auto c1 = line.find(',', 0); // find occurence of first comma
					if (c1 == string::npos) goto readnext; // if there's no comma in the line, skip to the next line
                    auto c2 = line.find(',', c1 + 1); // find occurence of second comma
					if (c2 == string::npos) goto readnext; // if there's no second comma in the line, skip to the next line
					{
						string s1 = line.substr(0, c1);
						string s2 = line.substr(c1 + 1, c2 - c1 - 1);
						string s3 = line.substr(c2 + 1);

						if (_stricmp(s1.c_str(), "set")==0) settings[s2] = s3;
					}
				}
	readnext:
				notEOF = (bool)getline(bidrulesfile, line);
			} while (notEOF);
		}
		catch (int e)
		{
			if (e==1) cout << "bid rules File was not found";
		}

        //assert(!true); // An error if we arrive here?
        return false;
	}

	void EA::ruleProcessing(ifstream& infile) {
		cout << "RULE PROCESSING STARTS";
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
					cout << "EXECUTES found" << endl;
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
						item.type = EA::SETTING_IMMEDIATE;
						thisRule.ruleSettings[s1] = item;
					}
					if (_stricmp(s1.c_str(), "probability") == 0) {
						// check if the value can be converted to float
						rule::settingItem item;
						item.value = s2;
						if (atoi(s2.c_str())>0 || isdigit(s2.c_str()[0]) ) {
							item.type = EA::SETTING_IMMEDIATE;
						} else {
							item.type = EA::SETTING_LOOKUP;
						}
						thisRule.ruleSettings[s1] = item;
					}
				} else {
					string s1 = line.substr(0, c1);
					string s2 = line.substr(c1 + 1, c2 - c1 - 1);
					string s3 = line.substr(c2 + 1);

					if (!inExecute) { // we do not have a line in the EXECUTE section
						//construct rule object line by line with one ruleline object per line
						cout << "standard item" << endl;
						rule::ruleLine rl;
						rl.valueToCheck = s1;
						rl.currentValue = s3;
						if (atoi(s3.c_str()) > 0 || isdigit(s3.c_str()[0])) {
							rl.type = EA::SETTING_IMMEDIATE;
						} else {
							rl.type = EA::SETTING_LOOKUP;
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
						if (_stricmp(s2.c_str(), "not") == 0) {
							rl.iOperator = rule::ruleLine::NOT;
						}
						thisRule.ruleLines.push_back(rl);
					} else {
						// We have an EXECUTE line
						thisRule.execute.probability = atoi(s1.c_str());
						if (_stricmp(s2.c_str(), "bid") == 0) thisRule.execute.action = EA::bidBID;
						if (_stricmp(s2.c_str(), "ea") == 0) thisRule.execute.action = EA::bidEA;
						thisRule.execute.value = s3.c_str();

					}
				}
			}
			notEOF = getline (infile, line);
		} while (notEOF);
	}

    bool EA::readRuleFile(string filename) {
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
            if (e==1) cout << "bidnet File was not found";
        }
        return true;
    }

	float EA::getEA(int cardsCount, int nonImportant, string cardsConfig, int bid) {
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

	EA::oneHand *EA::getEAstruct(int cardsCount, int nonImportant, string cardsConfig) {
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

    float EA::oneHand::get_eavalue(int bid) { return eavalues[bid - 6]; }

    int EA::getBidRoleInRound()
    {
        // make sure these two values are set and up to date
        gameSettings["harPacko"] = theGame->get_who_has_deck_pos_absolute();
        gameSettings["attBjuda"] = theGame->get_who_bids_pos_absolute();
        // calculate position and return it
        if (gameSettings["attBjuda"] + 0 % 4 == gameSettings["harPacko"]) return EA::PL_B_BLOCK;
        if (gameSettings["attBjuda"] + 1 % 4 == gameSettings["harPacko"]) return EA::PL_RESPONSE;
        if (gameSettings["attBjuda"] + 2 % 4 == gameSettings["harPacko"]) return EA::PL_A_BLOCK;
        if (gameSettings["attBjuda"] + 3 % 4 == gameSettings["harPacko"]) return EA::PL_LEAD;
        assert(false);
        return -1;
    }

    int EA::getBidPartner()
    {
        return theGame->get_bid_value_pos_relative_to_me(me, 2);
    }

    int EA::getBidRight()
    {
        return theGame->get_bid_value_pos_relative_to_me(me, 3);
    }

    //void EA::examineHand(QList<Card *> hand)
    cardsConfig EA::examineHand(list<Card *> hand)
    {
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
                LOG_D(hand.size());
                // use this loop to reset the search keys
                no_of_cards[suit] = 0;
                no_of_unimportant_cards[suit] = 0;
                str_card_search[suit]="";
                list<Card *>::iterator hand_iterator;
                for (hand_iterator=hand.begin(); hand_iterator != hand.end(); hand_iterator++)
                {
                    LOG_D(((Card *)(*hand_iterator))->card_ranking(suit));

                    if (((Card *)(*hand_iterator))->card_ranking(suit) > 0)
                    {
                        // Show the card
                        LOG_D(((Card *)(*hand_iterator))->card_suit_short() + "_" + (*hand_iterator)->card_face_value() + " ");
                        LOG_D( "  Ranking Name " + ((Card *)(*hand_iterator))->card_ranking_name(suit) + "  " );
                        array_hand[suit].push_back(((Card *)(*hand_iterator)));
                        //array_hand[suit].append(hand.at(j));
                    }
                }
//                for (int j = 0; j < hand.size(); ++j)
//                {
//                    if (hand.at(j)->card_ranking(suit)>0){
                        // Show the card
//                        LOG_D( hand.at(j)->card_suit_short() + "_" + hand.at(j)->card_face_value()+ " ");
//                        LOG_D( "  Ranking Name " + hand.at(j)->card_ranking_name(suit) + "  " );
//                        array_hand[suit].append(hand.at(j));
//                    }
//                }
                 LOG_D("");
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
//                      for (int i = 0; i < array_hand[suit].size(); ++i){
//                          if (array_hand[suit].at(i)->card_ranking(suit) ==  i_rank_to_remove)
//                          {
//                              LOG_D("remove card with rank ");
//                              LOG_D(i_rank_to_remove);
//                              array_hand[suit].removeAt(i);
//                              i_cards_to_remove--;
//                          }
//                      }
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
            LOG_D("");

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
                   int itmp = ((Card *)(*arrayHandIterator))->card_ranking(suit);
                   if (array_hand[suit].size() >= 4)
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
//               for (int i = 0; i < array_hand[suit].size(); ++i)
//               {
//                   if (array_hand[suit].size() >=4)
//                   {
//                       int itmp = array_hand[suit].at(i)->card_ranking(suit);  // pick the cards ranking
//                       if (array_hand[suit].size()==4)
//                       {
//                           // The cards [9,8,7,6] is considered as M
//                           if (itmp == 9 or itmp == 8 or itmp == 7 or itmp == 6)
//                           {
//                               no_of_unimportant_cards[suit]++;
//                           }
//                       }
//                       else  // this is the syntax for 5 and 6 cards
//                       {
//                           // The cards [9,8,7,6, 4, 3] is considered as M
//                           if (itmp == 9 or itmp == 8 or itmp == 7 or itmp == 6 or itmp == 3 or itmp == 2) // ranking values of 4 is 3 (and 3 is 2)
//                           {
//                               no_of_unimportant_cards[suit]++;
//                           }
//                       }
//                   }
//               }

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
//                       LOG_D(((Card *)(*handIterator))->card_face_value());
//                       LOG_D(((Card *)(*handIterator))->card_points);
//                       LOG_D(((Card *)(*handIterator))->card_ranking(suit));
//                       LOG_D(((Card *)(*handIterator))->card_ranking_name(suit));
//                       LOG_D(((Card *)(*handIterator))->card_suit());
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

//                   // inner loop of the cards per suit  - if a card is found it should be added
//                   for (int i = 0; i < array_hand[suit].size(); ++i)
//                   {
//                       if (array_hand[suit].at(i)->card_ranking(suit) == j)
//                           // Pick the topmost card and analyse
//                       {
//                           if (array_hand[suit].size() == 4 and (j == 9 or j == 8 or j == 7 or j == 6))
//                           {
//                              // Do nothing - ignore these cards
//                           }
//                           else
//                           {
//                               if (array_hand[suit].size() > 4 and (j == 9 or j == 8 or j == 7 or j == 6 or j == 3 or j == 2))
//                               {
//                                  // Do nothing - ignore these cards
//                               }
//                               else
//                               {
//                                   // jut put the string together
//                                   str_card_search[suit] += array_hand[suit].at(i)->card_ranking_name(suit);
//                                   str_card_search[suit] += "_";
//                               }
//                           }
//                       }
//                   }
               }

               str_card_search[suit] += "]";
               LOG_D( no_of_cards[suit]);
               LOG_D("       ");
               LOG_D( no_of_unimportant_cards[suit]);
               LOG_D("       ");
               LOG_D(str_card_search[suit]);

            }
            cardsConfig ret;
            for (int suit = 0; suit < 4; suit++) {
                ret.number_of_cards[suit] = no_of_cards[suit];
                ret.number_of_unimportant_cards[suit] = no_of_unimportant_cards[suit];
                ret.cardsString[suit]=str_card_search[suit];
            }
            return ret;
        }

	bidHint EA::getBidHint()
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

        // iterator through rules
		list<EA::rule>::iterator it;
		bool match;
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
			//check each rule for a match and then return on first match
			list<EA::rule::ruleLine>::iterator line_it;
			for (line_it = it->ruleLines.begin(); line_it != it->ruleLines.end(); line_it++) {
				match = true;
				string value;
				switch (line_it->type) {
				case EA::SETTING_IMMEDIATE: 
					value = line_it->currentValue; 
					break;
				case EA::SETTING_LOOKUP:
					value = it->ruleSettings[line_it->currentValue].value;
					break;
				}
				switch (line_it->iOperator) { 
				case EA::rule::ruleLine::SET: 
					it->ruleSettings[line_it->valueToCheck].value = value;
					it->ruleSettings[line_it->valueToCheck].type = EA::SETTING_IMMEDIATE;
					if (DEBUG) {
						cout << "* SET   " << line_it->valueToCheck << " to " << value << endl;
					}
					break;
				case EA::rule::ruleLine::GT:
					if (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()) <= atoi(value.c_str())) match = false;
					if (DEBUG) {
						cout << "* CHECK " << line_it->valueToCheck << " >  " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")" << endl;
					}
					break;
				case EA::rule::ruleLine::LT:
					if (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()) >= atoi(value.c_str())) match = false;
					if (DEBUG) {
						cout << "* CHECK " << line_it->valueToCheck << " <  " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")" << endl;
					}
					break;
				case EA::rule::ruleLine::NOT:
					if (atoi(it->ruleSettings[line_it->valueToCheck].value.c_str()) == atoi(value.c_str())) match = false;
					if (DEBUG) {
						cout << "* CHECK " << line_it->valueToCheck << " <> " << value << " (" << line_it->valueToCheck << " = " << it->ruleSettings[line_it->valueToCheck].value << ")" << endl;
					}
					break;

				default:
					cout << "NO OPERATOR MATCHES!" << endl;
					break;
				}
			}
			if (match) {
				cout << "Rule " << it->ruleSettings["Name"].value << " matched.";
				be = it->execute;
				break;
			} else {
				cout << "Rule " << it->ruleSettings["Name"].value << " did not match." << endl;
				cout << "-------------------------------" << endl;
			}
		}

        cardsConfig cards = examineHand(theGame->get_my_cards_in_hand(me));
        oneHand *oh[4];
        for (int suit=0; suit<4; suit++)
        {
            oh[suit]=getEAstruct(cards.number_of_cards[suit], cards.number_of_unimportant_cards[suit], cards.cardsString[suit]);
        }
		//bidExecute be;
		//be.action = 0;
		//be.probability = 100;
		//be.value = 10;
		return be;
	}


}
