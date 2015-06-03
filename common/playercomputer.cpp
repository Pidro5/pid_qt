#include <pidro/playercomputer.h>
#include <cassert>
#include <pidro/bidengine.h>
#include <pidro/playengine.h>

using namespace std;

namespace Pidro
{
  
PlayerComputer::PlayerComputer(const string& str)
    : Player(str)
{
}

PlayerComputer::~PlayerComputer()
{
}

void PlayerComputer::attached_to_game(Game* pGame, int position, bool rotate_to_south)
{
    m_pGame = pGame;
    m_position = position;
    m_rotate_to_south = rotate_to_south;

    //ea = new EA(g, this,
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\Bidnet76Clean.txt",
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\PidroBidRulesAI_Master013v1.txt");
    char* pDir = getenv("PIDRO_AI_DIR");

    string dir(pDir ? pDir : "C:");
    string rulesFile(dir + "\\Bidnet76Clean.txt");
    string bidRulesFile(dir + "\\PidroBidRulesAI.lua");

    string playRulesFile(dir + "\\PidroPlayRulesAI.lua");
//    string playRulesFile("C:\\_Pidro\\AI_used\\PlaySnippet.lua");

    m_sBidMachine = make_shared<BidEngine>(pGame, this, rulesFile, bidRulesFile);
    m_sPlayMachine = make_shared<PlayEngine>(pGame, this, playRulesFile);

}

bool PlayerComputer::inform_event(Event)
{
    return true;
}

bool PlayerComputer::inform_event(Event, int)
{
    return true;
}

bool PlayerComputer::inform_event(Event, int, int)
{
    return true;
}

bool PlayerComputer::inform_event(Event, int, std::list<Card *>&)
{
    return true;
}

int PlayerComputer::give_bid(int minimum)
{
    // minimum is the currect higest bid
    // if 0 or -1  the next highest bid is 6
    // if minimum is 14, the player can exceed this with a bid of 14

    assert(m_sBidMachine.get());
    return m_sBidMachine->give_bid(minimum);


}

int PlayerComputer::give_color()
{

    assert(m_sBidMachine.get());
    return m_sBidMachine->give_color();

}


Card* PlayerComputer::play_card(int color){
    Card* pC;

    assert( m_sPlayMachine.get());
    pC =   m_sPlayMachine->play_card( color);


    //assert(!pC);
    return pC;
}

}

