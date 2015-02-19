#include <QCoreApplication>
#include <iostream>

#include "deck_card.h"
#include "log.h"
#include "game.h"
#include "player.h"
#include "playerconsole.h"
#include "rotator.h"
#include "ea/ea.h"
#include "playercomputer.h"


using namespace std;


//void TEST_EA()
//{
//    Game g;
//    PlayerComputer p1("Alice");
//    PlayerComputer p2("Bob");
//    PlayerComputer p3("Cedric");
//    PlayerComputer p4("Diane");



//    LOG_D("");

//    g.register_with_game(&p1);
//    g.register_with_game(&p2);
//    g.register_with_game(&p3);
//    g.register_with_game(&p4);

//    EA ea(&g, &p1, "T:\\_home\\SchachtnerTh\\RulesFiles\\Bidnet76Clean.txt", "T:\\_home\\SchachtnerTh\\RulesFiles\\PidroBidRulesAI_Master013v1.txt");
////    EA ea(&g, &p1, "C:\\Bidnet76Clean.txt", "C:\\PidroBidRulesAI_Master013v1.txt");

//    cout << "EA test" << endl;
//    bidHint hint = ea.getBidHint();
//    cout << "EA test end" << endl;

//}


void TEST_PATRIK() {
    Game g;
    Player p1("Alice");
    PlayerConsole p2("Bob");
    PlayerComputer p3("Cedric");
    Player p4("Diane");

    LOG_D("");

    g.register_with_game(&p1);
    g.register_with_game(&p2,true);
    g.register_with_game(&p3);
    g.register_with_game(&p4);

    g.run_game();


}


//  MAIN ==============================================================================

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //TEST_EA();
    TEST_PATRIK();


    LOG_D("");
    LOG_D("MAIN DONE!");

    return a.exec();
}
