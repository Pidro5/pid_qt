#include "gamethread.h"
#include <pidro/game.h>
#include <pidro/log.h>
#include <pidro/playercomputer.h>
#include "playerproxy.h"
#include <QDir>

GameThread::GameThread(QPlayer* pPlayer)
    : m_pPlayer(pPlayer)
{
}

void GameThread::run()
{
    LOG_D("GameThread running");
    Pidro::Game g;

    // TODO: Use PIDRO_ROOT.
    g.set_game_path(QDir::currentPath().toStdString());

    // QDir::setCurrent("../qtui");
    
    PlayerProxy victor("Victor", m_pPlayer);
    Pidro::PlayerComputer alice("Alice");
    Pidro::PlayerComputer bob("Bob");
    Pidro::PlayerComputer carlos("Carlos");

    g.register_with_game(&victor, true);
    g.register_with_game(&alice);
    g.register_with_game(&bob);
    g.register_with_game(&carlos);

    g.run_game();
}
