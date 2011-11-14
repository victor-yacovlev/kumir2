#ifndef KUMIRCODERUN_FASTSTACK_H
#define KUMIRCODERUN_FASTSTACK_H

#include <stdlib.h>
#include <QtGlobal>
#include <QVector>

namespace KumirCodeRun {

template <class T> class FastStack
{
public:
    inline void init(int initialSize, int delta)
    {
        Q_ASSERT(initialSize>0);
        Q_ASSERT(delta>0);
        i_initialSize = initialSize;
        i_deltaSize = delta;
        i_currentIndex = -1;
        p_data = QVector<T>(initialSize);
    }

    inline void push(const T& t)
    {
        i_currentIndex ++;
        if (i_currentIndex>=p_data.size()) {
            p_data.resize(p_data.size()+i_deltaSize);
        }
        p_data[i_currentIndex] = t;
    }

    inline T pop()
    {
        i_currentIndex--;
        return p_data[i_currentIndex+1];
    }

    inline T& top()
    {
        return p_data[i_currentIndex];
    }

    inline const T& top() const
    {
        return p_data[i_currentIndex];
    }

    inline T& at(int index)
    {
        return p_data[index];
    }

    inline const T& at(int index) const
    {
        return p_data[index];
    }

    inline int size() const { return i_currentIndex+1; }

    inline void reset()
    {
        p_data = QVector<T>(i_initialSize);
        i_currentIndex = -1;
    }

    inline ~FastStack()
    {
        p_data.clear();
    }

private:
    int i_initialSize;
    int i_deltaSize;
    int i_currentIndex;
    QVector<T> p_data;
};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_FASTSTACK_H
