//
// Created by angel on 03/08/2019.
//

#include "DequeThreadSafe.h"

template<typename T>
void DequeThreadSafe<T>::push_back(T&& t)
{
    std::unique_lock<std::mutex> lck(mtx);
    while (deq.size() == maxsize && !finish)
        cv_full.wait(lck);
    assert(!finish);
    deq.push_back(std::move(t));
    cv_empty.notify_one();
}

template<typename T>
void DequeThreadSafe<T>::push_back(const T& t)
{
    std::unique_lock<std::mutex> lck(mtx);
    while (deq.size() == maxsize && !finish)
        cv_full.wait(lck);
    assert(!finish);
    deq.push_back(std::move(t));
    cv_empty.notify_one();
}

template<typename T>
std::optional<T> DequeThreadSafe<T>::pop()
{
    std::unique_lock<std::mutex> lck(mtx);
    while (deq.empty() && !finish)
        cv_empty.wait(lck);
    if (deq.empty()) return {};
    T t = std::move(deq.front());
    deq.pop();
    cv_full.notify_one();
    return t;
}

template<typename T>
void DequeThreadSafe<T>::close()
{
    finish = true;
    std::lock_guard<std::mutex> lck(mtx);
    cv_empty.notify_one();
    cv_full.notify_one();
}

template<typename T>
void DequeThreadSafe<T>::clear() {
    std::lock_guard<std::mutex> lck(mtx);
    deq.clear();
    cv_full.notify_all();
}


template<typename T>
typename std::deque<T, std::allocator<T>>::iterator DequeThreadSafe<T>::begin() {
    return deq.begin();
}

template<typename T>
typename std::deque<T, std::allocator<T>>::iterator DequeThreadSafe<T>::end() {
    return deq.end();
}

template<typename T>
typename std::deque<T, std::allocator<T>>::iterator DequeThreadSafe<T>::crbegin() {
    return deq.crbegin();
}

template<typename T>
typename std::deque<T, std::allocator<T>>::iterator DequeThreadSafe<T>::crend() {
    return deq.crend();
}



