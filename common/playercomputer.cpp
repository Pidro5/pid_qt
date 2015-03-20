#include "playercomputer.h"
#include <cassert>
#include "bidengine.h"

using namespace std;

PlayerComputer::PlayerComputer(const string& str)
    : Player(str)
{
}

PlayerComputer::~PlayerComputer()
{
}

void PlayerComputer::attached_to_game(Game* pGame, int position, bool rotate_to_south)
{
    m_my_game = pGame;
    m_my_position = position;
    m_my_rotate_to_south = rotate_to_south;

    //ea = new EA(g, this,
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\Bidnet76Clean.txt",
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\PidroBidRulesAI_Master013v1.txt");
    char* pDir = getenv("PIDRO_AI_DIR");

    string dir(pDir ? pDir : "C:");
    string rulesFile(dir + "\\Bidnet76Clean.txt");
    string bidRulesFile(dir + "\\PidroBidRulesAI.lua");

    m_sBidMachine = make_shared<BidEngine>(pGame, this, rulesFile, bidRulesFile);
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
