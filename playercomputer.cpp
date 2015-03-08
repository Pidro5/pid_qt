#include "playercomputer.h"
#include "player.h"
#include "ea/ea.h"

EA *ea;


PlayerComputer::PlayerComputer(string str) : Player(str)
{
}

PlayerComputer::~PlayerComputer()
{
    delete ea;
}

void PlayerComputer::attached_to_game(Game * g, int position, bool rotate_to_south)
{
    my_game = g;
    my_position = position;
    my_rotate_to_south = rotate_to_south;
    //ea = new EA(g, this,
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\Bidnet76Clean.txt",
    //            "T:\\_home\\SchachtnerTh\\RulesFiles\\PidroBidRulesAI_Master013v1.txt");
    char* pDir = getenv("PIDRO_AI_DIR");
    string dir(pDir ? pDir : "C:");

    ea = new EA(g, this, dir + "\\Bidnet76Clean.txt", dir + "\\PidroBidRulesAI_Master013v1.txt");
}

int PlayerComputer::give_bid(int minimum){

    // minimum is the currect higest bid
    // if 0 or -1  the next highest bid is 6
    // if minimum is 14, the player can exceed this with a bid of 14

    ea->getBidHint();

    if (minimum ==5) {return 6;}

    int rnd = rand() % 10;
    if (rnd < 5) {return 0;}     //  50% to pass

    if (minimum ==0) {return 10;}
    if (minimum ==14) {return 14;}
    return minimum+1;
}

int PlayerComputer::give_color(){
    // color can be 0 ..3
    int rnd = rand() % 4;

    return rnd;
}
