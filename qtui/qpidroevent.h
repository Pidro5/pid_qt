#ifndef QPIDROEVENT_H
#define QPIDROEVENT_H

#include <list>
#include <memory>
#include <QEvent>
#include <QSemaphore>
#include <pidro/deck_card.h>
#include <pidro/event.h>
#include "qpidroresult.h"

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

    virtual void deliverTo(QPlayer& player) = 0;

protected:
    QPidroEvent();
};

template<class T>
class QPidroEventT : public QPidroEvent
{
public:
    typedef QPidroResultT<T> Result;

    QPidroEventT(const QPidroEventT&) = delete;
    QPidroEventT& operator = (const QPidroEventT&) = delete;

    std::shared_ptr<Result> result() const {
        return m_sResult;
    }

protected:
    QPidroEventT(std::shared_ptr<Result> sResult)
        : m_sResult(sResult)
    {}

private:
    std::shared_ptr<Result> m_sResult;
};

/*
 * QPidroInfoEvent
 */
class QPidroInfoEvent : public QPidroEventT<bool>
{
public:
    QPidroInfoEvent(const QPidroInfoEvent&) = delete;
    QPidroInfoEvent& operator = (const QPidroInfoEvent&) = delete;

    void deliverTo(QPlayer& player);

protected:
    QPidroInfoEvent(std::shared_ptr<QPidroResultBool> sResult,
                    Pidro::Event event);

    virtual bool doDeliverTo(QPlayer& player) = 0;

protected:
    const Pidro::Event m_event;
};

/*
 * QPidroInfoEvent1
 */
class QPidroInfoEvent1 : public QPidroInfoEvent
{
public:
    QPidroInfoEvent1(std::shared_ptr<QPidroResultBool> sResult, Pidro::Event ev);
    QPidroInfoEvent1(const QPidroInfoEvent1&) = delete;
    QPidroInfoEvent1& operator = (const QPidroInfoEvent1&) = delete;

private:
    bool doDeliverTo(QPlayer& pPlayer);
};

/*
 * QPidroInfoEvent2
 */
class QPidroInfoEvent2 : public QPidroInfoEvent
{
public:
    QPidroInfoEvent2(std::shared_ptr<QPidroResultBool> sResult, Pidro::Event ev, int position);
    QPidroInfoEvent2(const QPidroInfoEvent2&) = delete;
    QPidroInfoEvent2& operator = (const QPidroInfoEvent2&) = delete;

private:
    bool doDeliverTo(QPlayer& player) override;

private:
    int m_position;
};

/*
 * QPidroInfoEvent3
 */
class QPidroInfoEvent3 : public QPidroInfoEvent
{
public:
    QPidroInfoEvent3(std::shared_ptr<QPidroResultBool> sResult, Pidro::Event ev, int position, int value);
    QPidroInfoEvent3(const QPidroInfoEvent3&) = delete;
    QPidroInfoEvent3& operator = (const QPidroInfoEvent3&) = delete;

private:
    bool doDeliverTo(QPlayer& player) override;

private:
    int m_position;
    int m_value;
};

/*
 * QPidroInfoEvent4
 */
class QPidroInfoEvent4 : public QPidroInfoEvent
{
public:
    QPidroInfoEvent4(std::shared_ptr<QPidroResultBool> sResult,
                     Pidro::Event ev,
                     int position,
                     std::list<Pidro::Card*>& cards);
    QPidroInfoEvent4(const QPidroInfoEvent4&) = delete;
    QPidroInfoEvent4& operator = (const QPidroInfoEvent4&) = delete;

private:
    bool doDeliverTo(QPlayer& player) override;

private:
    int m_position;
    std::list<Pidro::Card*> m_cards; // TODO: Could be reference.
};

/*
 * QPidroCommandGiveBid
 */
class QPidroCommandGiveBid : public QPidroEventT<int>
{
public:
    QPidroCommandGiveBid(std::shared_ptr<QPidroResultInt> sResult,
                         int minimum);
    QPidroCommandGiveBid(const QPidroCommandGiveBid&) = delete;
    QPidroCommandGiveBid& operator = (const QPidroCommandGiveBid&) = delete;

    void deliverTo(QPlayer& player);

private:
    int m_minimum;
};

/*
 * QPidroCommandGiveColor
 */
class QPidroCommandGiveColor : public QPidroEventT<int>
{
public:
    QPidroCommandGiveColor(std::shared_ptr<QPidroResultInt> sResult);
    QPidroCommandGiveColor(const QPidroCommandGiveColor&) = delete;
    QPidroCommandGiveColor& operator = (const QPidroCommandGiveColor&) = delete;

    void deliverTo(QPlayer& player);
};

/*
 * QPidroCommandPlayCard
 */
class QPidroCommandPlayCard : public QPidroEventT<Pidro::Card*>
{
public:
    QPidroCommandPlayCard(std::shared_ptr<QPidroResultCard> sResult, int color);
    QPidroCommandPlayCard(const QPidroCommandPlayCard&) = delete;
    QPidroCommandPlayCard& operator = (const QPidroCommandPlayCard&) = delete;

    void deliverTo(QPlayer& player);

private:
    int m_color;
};


#endif // QPIDROEVENT_H

