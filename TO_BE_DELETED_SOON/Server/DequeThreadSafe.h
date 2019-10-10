//
// Created by angel on 03/08/2019.
//

#ifndef SERVER_DEQUETHREADSAFE_H
#define SERVER_DEQUETHREADSAFE_H

#include <cstdio>
#include <utility>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <optional>
#include <cassert>

template <typename T>
class DequeThreadSafe {
public:
    /* Create DequeThreadSafe object. Set maximum size of the queue to max_size. */
    inline DequeThreadSafe(size_t max_size = -1UL) : maxsize(max_size), finish(false) {};  //todo verificare se necessario avere un massimo

    /* Push T to the deque. Many threads can push at the same time.
     * If the queue is full, calling thread will be suspended until
     * some other thread pop() data. */
    void push_back(const T&);
    void push_back(T&&);

    /* Close the deque.
     * Be sure all writing threads done their writes before call this.
     * Push data to closed queue is forbidden. */
    void close();

    /* Pop and return T from the queue. Many threads can pop at the same time.
     * If the queue is empty, calling thread will be suspended.
     * If the queue is empty and closed, nullopt returned. */
    std::optional<T> pop();

    /* Clear the deque
     */
    void clear();

    //std::deque<T> getDeque();
    typename std::deque<T>::iterator begin();
    typename std::deque<T>::iterator end();
    typename std::deque<T>::iterator crbegin();
    typename std::deque<T>::iterator crend();

private:
    std::deque<T> deq;
    std::mutex mtx;
    std::condition_variable cv_empty, cv_full;
    const size_t maxsize;
    std::atomic<bool> finish;
};


#endif //SERVER_DEQUETHREADSAFE_H
