#ifndef QPIDRORESULT_H
#define QPIDRORESULT_H

#include <QSemaphore>

class QPidroResult
{
public:
    QPidroResult(bool initial = false);
    QPidroResult(const QPidroResult&) = delete;
    QPidroResult& operator = (const QPidroResult&) = delete;
    ~QPidroResult();

    void wait();

    void setCode(bool code);

    bool code() const;

private:
    QSemaphore m_sem;
    bool m_code;
};

#endif // QPIDRORESULT_H
