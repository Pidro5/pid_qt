#include <QCoreApplication>
#include <iostream>

#include "common/game.h"
#include "common/player.h"
#include "cli/playerconsole.h"
#include "common/playercomputer.h"


using namespace std;


//  MAIN ==============================================================================

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Game g;
    PlayerComputer p1("Alice");
    PlayerConsole p2("Bob");
    PlayerComputer p3("Cedric");
    PlayerComputer p4("Diane");


    g.register_with_game(&p1);
    g.register_with_game(&p2,true);
    g.register_with_game(&p3);
    g.register_with_game(&p4);

    g.run_game();


    LOG_D("");
    LOG_D("MAIN DONE!");

    return a.exec();
}
