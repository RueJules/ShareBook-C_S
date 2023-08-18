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
    ThreadSafe_queue(){}
    ThreadSafe_queue(ThreadSafe_queue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data = other.data;
    }
    void push(T new_val)
    {
        std::lock_guard<std::mutex> lk(mut);
        data.push(new_val);
        convar.notify_one();
    }
    void wait_and_pop(T& val)
    {
        std::unique_lock<std::mutex> lk(mut);
        convar.wait(lk,[this]{return !data.empty();});
        val = data.front();
        data.pop();


    }
//    void wait_and_pop(std::pair<T, L> &kv)
//    {
//        std::unique_lock<std::mutex> lk(mut);
//        convar.wait(lk,[this]{return !data_map.empty();});
//        kv = std::pair<T, L>{data_map.begin()->first, data_map.begin()->second};
//        data_map.erase(data_map.begin());


//    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        convar.wait(lk,[this]{return !data.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data.front()));
        data.pop();
        return res;
    }
    bool try_pop(T& val) //非阻塞的
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data.empty())
            return false;
        val = data.front();
        data.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if(data.empty())
            return std::make_shared<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data.front()));
        data.pop();
        return res;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data.empty();
    }

private:
    mutable std::mutex mut;
    std::queue<T> data;
    std::condition_variable convar;


};

#endif // THREADSAFE_QUEUE_H
