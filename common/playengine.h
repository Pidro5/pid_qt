#ifndef PLAYENGINE_H
#define PLAYENGINE_H

#include "game.h"
#include "player.h"

extern "C" {
#include "luah/lua.h"        //Lua main library (lua_*)
#include "luah/lauxlib.h"
#include "luah/lualib.h"
}

class PlayData
{
private:
    Game *m_theGame;
    Player *m_me;

    int m_MyCards[6];
    int m_MyCardsNumber = 0;    // how mny cards
    int m_PlayedCards[14];
    int m_PlayedCardsNumber = 0;  // how mny cards
    bool m_ColdClean[4];
    bool m_ColdCalculated[4];
    int m_Bid = 0;
    int m_Cardleft = 0;
    int m_Cardright = 0;
    int m_Cardpartner = 0;

    int m_Round;
    int m_RoleBid;   // refers to Game::BIDDER...
    int m_CardHighestAbsolute = 0;
    int m_PointsLeft = 0;
    int m_PreviousCardLeft = 0;
    int m_PreviousCardPartner = 0;
    int m_PreviousCardRight = 0;
    int m_PreviousCardMe = 0;
    int m_PreviousCardHighestRound = 0;
    int m_PreviousCardHighestAbsolute = 0;
    int m_WhoPlays = 0;
    int m_StartsRound = 0;
    int  m_CardsInitiallyInMyHand = 0;
    int m_CardsBeforeBuyingLeft = 0;
    int m_CardsBeforeBuyingPartner = 0;
    int m_CardsBeforeBuyingRight = 0;
    int m_CardsLoose = 0;
    int m_CardsUnknown = 0;

    float m_CardsExpectedPartner = 0;
    float m_CardsExpectedLeft = 0;
    float m_CardsExpectedRight = 0;
    float m_CardsExpectedUs = 0;
    float m_CardsExpectedThem = 0;
    float m_CardsExpectedThemMax = 0;

    int m_PointsTakenUs = 0;
    int m_PointsTakenThem = 0;
    int m_CardsBeforeBuyingThemMax = 0;
    int m_PointsToKill = 0;

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

    bool EqualInRank(int rank1, int rank2);
    int GetPointsToKill();
    int GetCardsLoose();

    bool GetNotColdFourthTurnThem();
    bool GetNotColdFifthTurnThem();
    bool GetNotColdSixthTurnThem();

    bool GetNotSafeFifthTurn();
    bool GetNotSafeSixthTurn();

    bool GetSafeFifthTurn();
    bool GetSafeSixthTurn();

    bool GetNotColdFifthTurnThemExpected();
    bool GetNotColdSixthTurnThemExpected();

    float GetCardsExpectedLeft();
    float GetCardsExpectedPartner();
    float GetCardsExpectedRight();
    float GetCardsExpectedThem();
    float GetCardsExpectedThemMax();
    float GetCardsExpectedUs();

public:
    PlayData(Game *game, Player *player);
    void populate_with_data(int color);
    void push_to_lua(lua_State *m_L);

};

class PlayEngine
{    
private:
    Game *m_theGame;
    Player *m_me;
    char m_playRuleFile[FILENAME_MAX];

    // handle to lua
    lua_State *m_L;
public:
    PlayEngine(Game *game, Player *player, std::string playrulefile);
    ~PlayEngine();
    Card*  play_card(int color);

};

#endif // PLAYENGINE_H
