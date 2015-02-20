#include <QCoreApplication>
#include <iostream>

#include "deck_card.h"
#include "log.h"
#include "game.h"
#include "rotator.h"
#include "player.h"
#include "playerconsole.h"
#include "playercomputer.h"


using namespace std;


//  MAIN ==============================================================================

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Game g;
    Player p1("Alice");
    PlayerConsole p2("Bob");
    PlayerComputer p3("Cedric");
    Player p4("Diane");


    g.register_with_game(&p1);
    g.register_with_game(&p2,true);
    g.register_with_game(&p3);
    g.register_with_game(&p4);

    g.run_game();


    LOG_D("");
    LOG_D("MAIN DONE!");

    return a.exec();
}
