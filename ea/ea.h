#ifndef EA_EA_H
#define EA_EA_H

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <list>
#include <map>
#include <vector>

#include "game.h"
#include "player.h"

class bidHint {
public:
    int probability;
    int action;
    string value;
};

class cardsConfig {
public:
    int number_of_cards[4];
    int number_of_unimportant_cards[4];
    string cardsString[4];
};

class oneHand {
public:
    float eaNB;
    float eavalues[9];
    float probability[9];
    float eaNBs[4];

public:
    void set_eaNB(float value) { eaNB = value; }
    float get_eaNB() { return eaNB; }
    float *get_eavalues() { return eavalues; }
    float get_eavalue(int bid);
    float *get_probabilities() { return probability; }
    float *get_eaNBall() { return eaNBs; }
    void set_eaValues(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) {
        eavalues[0] = v1;
        eavalues[1] = v2;
        eavalues[2] = v3;
        eavalues[3] = v4;
        eavalues[4] = v5;
        eavalues[5] = v6;
        eavalues[6] = v7;
        eavalues[7] = v8;
        eavalues[8] = v9;
    }
    void set_probability(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) {
        probability[0] = v1;
        probability[1] = v2;
        probability[2] = v3;
        probability[3] = v4;
        probability[4] = v5;
        probability[5] = v6;
        probability[6] = v7;
        probability[7] = v8;
        probability[8] = v9;
    }
    void set_eaNBs(float v1,float v2,float v3,float v4) {
        eaNBs[0] = v1;
        eaNBs[1] = v2;
        eaNBs[2] = v3;
        eaNBs[3] = v4;
    }
    //           oneHand& operator=(oneHand other);
};


class EA {
public:
    const static int bidBID = 1; // used for bidExecute
    const static int bidEA = 2; // used for bidExecute
    const static int SETTING_IMMEDIATE = 1;
    const static int SETTING_LOOKUP = 2;
    const static int PL_LEAD = 1;
    const static int PL_RESPONSE = 2;
    const static int PL_A_BLOCK = 3;
    const static int PL_B_BLOCK = 4;
    const static int C_SUIT_SPADES = 0;
    const static int C_SUIT_CLUBS = 1;
    const static int C_SUIT_HEARTS = 2;
    const static int C_SUIT_DIAMONDS = 3;
private:
    Game *theGame;
    Player *me;
    void updateGameSettings();
    float getEABestColor(int bid);
    vector<string> &split(const string &s, char delim, vector<string> &elems);
    vector<string> split(const std::string &s, char delim);
    class rule {
    public:
        class settingItem {
        public:
            string value;
            int type;
        };
        class ruleLine {
        public:
            static const int SET=1;
            static const int LT=2;
            static const int GT=3;
            static const int NOT=4;
            static const int ONEOF=5;
            static const int CALC_ADD=6;
            static const int CALC_SUB=7;
            int iOperator;
            string valueToCheck;
            string currentValue;
            int type;
            bool isFunction;
            string argument;
            int argumentType;
            bool checkRuleMatch(int variable, int iOperator, int value);
        };
        map<string, settingItem> ruleSettings;
        list<ruleLine> ruleLines;
        //list<executeLine> executeLines;
        // Execute parameters
        bidHint execute;
    };

    char eaRuleFile[FILENAME_MAX];
    char eaBidRuleFile[FILENAME_MAX];

    map<std::string, string> settings;
    list<rule> rules;
    ifstream bidrulesfile;


    class cardcount{
    public:
        int numOfCards;
        int nonImportant;
        std::map<std::string, oneHand> cardCombos;
        std::map<std::string, oneHand>::iterator cardCombosIterator;
    };

    std::list<cardcount> cardcounts;
    bool readRuleFile(string str);
    bool readBidRuleFile(string filename);
    float getEA(int cardsCount, int nonImportant, string cardsConfig, int bid);
    oneHand *getEAstruct(int cardsCount, int nonImportant, string cardsConfig);
    void ruleProcessing(ifstream& infile);

    cardsConfig examineHand(list<Card *> hand);
    int getBidRoleInRound();
    int getBidPartner();
    int getBidRight();

    string bool_to_string(bool b);
public:
    EA(void);
    EA(string rulefile);
    EA(string rulefile, string bidRuleFile);
    EA(Game *game, Player *player, string rulefile, string bidRuleFile);
    bidHint getBidHint();
};

#endif
