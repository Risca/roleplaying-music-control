#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>


template <typename T> class ThreadSafeQueue
{
public:
    void put(T item) {
        QMutexLocker locker(&mtx);

        queue.append(item);
        cond.wakeOne();
    }

    bool get(T &item, unsigned long time = ULONG_MAX) {
        QMutexLocker locker(&mtx);

        while (queue.isEmpty()) {
            bool success = cond.wait(&mtx, time);
            if (!success) {
                // Timed out
                return false;
            }
        }

        item = queue.takeFirst();

        return true;
    }

private:
    QList<T> queue;
    QMutex mtx;
    QWaitCondition cond;
};

#endif // THREADSAFEQUEUE_H
