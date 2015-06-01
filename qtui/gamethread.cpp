#include "gamethread.h"
#include <game.h>
#include <log.h>
#include <playercomputer.h>
#include "playerproxy.h"


GameThread::GameThread(QPlayer* pPlayer)
    : m_pPlayer(pPlayer)
{
}

void GameThread::run()
{
    LOG_D("GameThread running");
    Pidro::Game g;
    
    Pidro::PlayerComputer p1("Alice");
    Pidro::PlayerComputer p2("Carlos");
    Pidro::PlayerComputer p3("Bob");
    PlayerProxy p4("Victor", m_pPlayer);

    g.register_with_game(&p1);
    g.register_with_game(&p2);
    g.register_with_game(&p3);
    g.register_with_game(&p4,true);

    g.run_game();
}
