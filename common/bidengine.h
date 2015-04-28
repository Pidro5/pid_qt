#ifndef BIDENGINE_H
#define BIDENGINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <list>
#include <map>
#include <vector>

#include "game.h"
#include "player.h"

extern "C" {
#include "luah/lua.h"        //Lua main library (lua_*)
#include "luah/lauxlib.h"
#include "luah/lualib.h"
}


class cardsConfig {
public:
    int m_number_of_cards[4];
    int m_number_of_unimportant_cards[4];
    string m_cardsString[4];
};

class oneHand {
public:
    float m_eavalues[9];
    float m_probability[9];
    float m_eaNBs[4];

public:
    float get_eavalue(int bid);
    void set_eaValues(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) {
        m_eavalues[0] = v1;
        m_eavalues[1] = v2;
        m_eavalues[2] = v3;
        m_eavalues[3] = v4;
        m_eavalues[4] = v5;
        m_eavalues[5] = v6;
        m_eavalues[6] = v7;
        m_eavalues[7] = v8;
        m_eavalues[8] = v9;
    }
    void set_probability(float v1,float v2,float v3,float v4,float v5,float v6,float v7,float v8,float v9) {
        m_probability[0] = v1;
        m_probability[1] = v2;
        m_probability[2] = v3;
        m_probability[3] = v4;
        m_probability[4] = v5;
        m_probability[5] = v6;
        m_probability[6] = v7;
        m_probability[7] = v8;
        m_probability[8] = v9;
    }
    void set_eaNBs(float v1,float v2,float v3,float v4) {
        m_eaNBs[0] = v1;
        m_eaNBs[1] = v2;
        m_eaNBs[2] = v3;
        m_eaNBs[3] = v4;
    }
};


class BidEngine {

private:
    Game *m_theGame;
    Player *m_me;

    // handle to lua
    lua_State *m_L;

    oneHand *m_myEAs[4];
    float getEABestValue(int bid)const;
    int getEABestColor(int bid) const;
    float getPLevel(int bid) const;
    int getPLevelColor(int bid) const;
    float getWorstEAuH(int pos) const;   //pos 0..3  South, West..

    int m_my_color;

    char m_eaFile[FILENAME_MAX];
    char m_eaBidRuleFile[FILENAME_MAX];

    class cardcount{
    public:
        int m_numOfCards;
        int m_nonImportant;
        std::map<std::string, oneHand> m_cardCombos;
        std::map<std::string, oneHand>::iterator m_cardCombosIterator;
    };

    std::list<cardcount> m_cardcounts;
    bool readEAFile(string str);
    float getEA(int cardsCount, int nonImportant, string cardsConfig, int bid);
    oneHand *getEAstruct(int cardsCount, int nonImportant, string cardsConfig);

    cardsConfig examineHand(list<Card *> hand);

    BidEngine(string rulefile, string bidRuleFile);

public:
    BidEngine(Game *game, Player *player, string rulefile, string bidRuleFile);
    ~BidEngine();
    int give_bid(int minimum);
    int give_color() const;
};

#endif   //BIDENGINE_H
