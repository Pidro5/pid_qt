#ifndef QPIDRORESULT_H
#define QPIDRORESULT_H

#include <QSemaphore>
#include <pidro/deck_card.h>

/*
 * QPidroResult
 */
class QPidroResult
{
public:
    QPidroResult(const QPidroResult&) = delete;
    QPidroResult& operator = (const QPidroResult&) = delete;
    ~QPidroResult();

    void wait();

protected:
    QPidroResult();

    void post();

private:
    QSemaphore m_sem;
};

/*
 * QPidroResultT
 */
template <class T>
class QPidroResultT : public QPidroResult
{
public:
    QPidroResultT()
        : m_value()
    {}
    QPidroResultT(const QPidroResultT&) = delete;
    QPidroResultT& operator = (const QPidroResultT&) = delete;

    void setValue(T value) {
        m_value = value;
        post();
    }

    T value() const {
        return m_value;
    }

private:
    T m_value;
};

typedef QPidroResultT<bool>         QPidroResultBool;
typedef QPidroResultT<int>          QPidroResultInt;
typedef QPidroResultT<Pidro::Card*> QPidroResultCard;

#endif // QPIDRORESULT_H
