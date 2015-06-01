#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QThread>

class QPlayer;

class GameThread : public QThread
{
public:
  GameThread(QPlayer* pPlayer);
  GameThread(const GameThread&) = delete;
  GameThread& operator = (const GameThread&);

private:
  void run() override;

private:
  QPlayer* m_pPlayer;
};

#endif
