#ifndef QPIDROEVENT_H
#define QPIDROEVENT_H

#include <list>
#include <memory>
#include <QEvent>
#include <QSemaphore>
#include <pidro/deck_card.h>
#include <pidro/event.h>

class QPlayer;
class QPidroResult;

/*
 * QPidroEvent
 */
class QPidroEvent : public QEvent
{
public:
    enum {
        Id = 4711
    };

    ~QPidroEvent();
    QPidroEvent(const QPidroEvent&) = delete;
    QPidroEvent& operator = (const QPidroEvent&) = delete;

    virtual bool deliverTo(QPlayer& pPlayer) = 0;

    std::shared_ptr<QPidroResult> result() const;

protected:
    QPidroEvent(std::shared_ptr<QPidroResult> sResult);

private:
    std::shared_ptr<QPidroResult> m_sResult;
};

/*
 * QPidroEvent1
 */
class QPidroEvent1 : public QPidroEvent
{
public:
    QPidroEvent1(std::shared_ptr<QPidroResult> sResult, Pidro::Event ev);
    QPidroEvent1(const QPidroEvent1&) = delete;
    QPidroEvent1& operator = (const QPidroEvent1&) = delete;

    bool deliverTo(QPlayer& pPlayer);

private:
    Pidro::Event m_event;
};

/*
 * QPidroEvent2
 */
class QPidroEvent2 : public QPidroEvent
{
public:
    QPidroEvent2(std::shared_ptr<QPidroResult> sResult, Pidro::Event ev, int position);
    QPidroEvent2(const QPidroEvent2&) = delete;
    QPidroEvent2& operator = (const QPidroEvent2&) = delete;

    bool deliverTo(QPlayer& pPlayer);

private:
    Pidro::Event m_event;
    int m_position;
};

/*
 * QPidroEvent3
 */
class QPidroEvent3 : public QPidroEvent
{
public:
    QPidroEvent3(std::shared_ptr<QPidroResult> sResult, Pidro::Event ev, int position, int value);
    QPidroEvent3(const QPidroEvent3&) = delete;
    QPidroEvent3& operator = (const QPidroEvent3&) = delete;

    bool deliverTo(QPlayer& pPlayer);

private:
    Pidro::Event m_event;
    int m_position;
    int m_value;
};

/*
 * QPidroEvent4
 */
class QPidroEvent4 : public QPidroEvent
{
public:
    QPidroEvent4(std::shared_ptr<QPidroResult> sResult,
                 Pidro::Event ev,
                 int position,
                 std::list<Pidro::Card*>& cards);
    QPidroEvent4(const QPidroEvent4&) = delete;
    QPidroEvent4& operator = (const QPidroEvent4&) = delete;

    bool deliverTo(QPlayer& pPlayer);

private:
    Pidro::Event m_event;
    int m_position;
    std::list<Pidro::Card*> m_cards; // TODO: Could be reference.
};

#endif // QPIDROEVENT_H

