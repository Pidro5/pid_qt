#include "gamethread.h"
#include <game.h>
#include <log.h>
#include <playercomputer.h>
#include "playerproxy.h"


GameThread::GameThread()
{
}

void GameThread::run()
{
    LOG_D("GameThread running");
    Game g;
    
    PlayerComputer p1("Alice");
    PlayerComputer p2("Carlos");
    PlayerComputer p3("Bob");
    PlayerProxy p4("Victor");

    g.register_with_game(&p1);
    g.register_with_game(&p2,true);
    g.register_with_game(&p3);
    g.register_with_game(&p4);

    g.run_game();
}
