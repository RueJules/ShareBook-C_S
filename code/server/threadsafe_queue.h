#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

template<typename T>
class ThreadSafe_queue
{
public:
    ThreadSafe_queue();
    ThreadSafe_queue(ThreadSafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }
    void push(T new_val)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_val);
        convar.notify_one();
    }
    void wait_and_pop(T& val)
    {
        std::lock_guard<std::mutex> lk(mut);
        convar.wait(lk,[this]{return !data_queue.empty()});
        val = data_queue.front();
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop()
    {

    }

private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable convar;


};

#endif // THREADSAFE_QUEUE_H
