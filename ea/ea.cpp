#include <cassert>
#include <string.h>
#include <stdlib.h>

#include "ea.h"
#include "game.h"
#include "player.h"

static const bool DEBUG = true;

namespace std {
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

	EA::EA(Game *game, Player *player, string rulefile, string bidRuleFile) {
		theGame=game;
		me=player;
		EA(rulefile, bidRuleFile);
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

        assert(!true); // An error if we arrive here?
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
					char *key = cardsConfig1 + 1;
					int i = strlen(key);
					key[i-1] = '\0';

					//cout << cardsConfig1+1;

					//for (int i = 0; key[i]; i++) { key[i] = toupper(key[i]); }

					cc.cardCombos[cardsConfig1+1] = oh;

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
		}
	}
	if (found) 
		return &(cc->cardCombos[cardsConfig]);
	else 
        return NULL;
}

	float EA::oneHand::get_eavalue(int bid) { return eavalues[bid - 6]; };


	bidHint EA::getBidHint()
	{
        map<string, string> gameSettings;
        gameSettings["VI"] = theGame->get_game_our_points(me);
        gameSettings["DE"] = theGame->get_game_their_points(me);
		gameSettings["Ledning"] = atoi(gameSettings["VI"].c_str()) - atoi(gameSettings["DE"].c_str());
        gameSettings["harPacko"] = theGame->get_who_has_deck_pos_absolute();
        gameSettings["attBjuda"] = theGame->get_who_bids_pos_absolute();
        //gameSettings["BidHighestRound"] = getBidHighestRound(atoi(gameSettings["harPacko"].c_str()), atoi(gameSettings["attBjuda"].c_str()));
        //gameSettings["BidPartner"] = theGame->get_bid_partner();
        // iterator through rules
		list<EA::rule>::iterator it;
		bool match;
		bidHint be;
		for (it = rules.begin(); it != rules.end(); it++)
		{
			// copy all items from the gameSettings structure into the ruleSettings map
			map<string, string>::iterator settingsIt;
			for (settingsIt = gameSettings.begin(); settingsIt != gameSettings.end(); settingsIt++) {
				it->ruleSettings[settingsIt->first].value = settingsIt->second;
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
		//bidExecute be;
		//be.action = 0;
		//be.probability = 100;
		//be.value = 10;
		return be;
	}


}
