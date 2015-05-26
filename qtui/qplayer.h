#ifndef QPLAYER_H
#define QPLAYER_H

#include <QObject>
#include <string>
#include "player.h"

class QPlayer : public QObject,
                private Player
{
    Q_OBJECT

public:
    explicit QPlayer(const std::string& name,
                     QObject* pParent = 0);
    ~QPlayer();

private:
    bool inform_event(Event et) override;
    bool inform_event(Event et, int position) override;
    bool inform_event(Event et, int position, int value) override;
    bool inform_event(Event et, int position, std::list<Card *>& cards) override;

signals:

public slots:
};

#endif // QPLAYER_H
