#ifndef FP_H
#define FP_H

#include <QtCore>

template<typename T, typename R> extern inline QList<R> MAP( R (*function)(const T&), const QList<T> &iterable)
{
    QList<R> result;
    for (int i=0; i<iterable.size(); i++) {
        result << (*function)(iterable[i]);
    }
    return result;
}

template<typename T> extern inline T REDUCE( T (*function)(const T&, const T&), const QList<T> &iterable)
{
    Q_ASSERT(!iterable.isEmpty());
    T result = iterable.first();
    for (int i=1; i<iterable.size(); i++) {
        result = (*function)(result, iterable[i]);
    }
    return result;
}

template<typename T> extern inline QList<T> FILTER( bool (*function)(const T&), const QList<T> &iterable)
{
    QList<T> result;
    for (int i=0; i<iterable.size(); i++) {
        if ((*function)(iterable[i])) {
            result << iterable[i];
        }
    }
    return result;
}

#endif // FP_H
