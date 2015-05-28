#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QThread>

class GameThread : public QThread
{
public:
  GameThread();
  GameThread(const GameThread&) = delete;
  GameThread& operator = (const GameThread&);

private:
  void run() override;
};

#endif
