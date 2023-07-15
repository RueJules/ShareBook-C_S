#ifndef CACHE_H
#define CACHE_H
#include <QString>
#include <memory>
#include <QList>
#include <unordered_map>
#include <QDebug>
#include <mutex>
#define FIND_COUNT 3

template<typename T>
class Cache
{
public:
    Cache() = default;
    explicit Cache(const std::unordered_map<QString,T> c) : cache(c) {}
    bool inCache(QString id){

        std::unique_lock<std::mutex> lock(cacheMutex);

        if(cache.count(id))
            return true;
        else
            return false;
    }
    std::unordered_map<QString,T>& getCacheConst(){

        std::unique_lock<std::mutex> lock(cacheMutex);

        return cache;
    }
    T& getFromCache(QString id){

        std::unique_lock<std::mutex> lock(cacheMutex);

        return cache.at(id);
    }
    void deleteFromCache(QString id){

        std::unique_lock<std::mutex> lock(cacheMutex);

        cache.erase(id);
    }
    void clearCache(){

        std::unique_lock<std::mutex> lock(cacheMutex);

        cache.clear();
    }
    void addToCache(QString id,T &&entity){

        std::unique_lock<std::mutex> lock(cacheMutex);

        //question:加入缓存中的数据无法读取。
        qDebug() << "cachesize:"<<cache.size() << '\n';
        cache.emplace_hint(cache.begin(),id,std::move(entity));
    }
    void  getSome(QList<QString> &listCompare, QList<T*> &list){

        std::unique_lock<std::mutex> lock(cacheMutex);

        qDebug() << "from cache size: -----" << cache.size()<<'\n';
        for(auto it = cache.begin(); it != cache.end(); ++it){
            if(list.length() >= FIND_COUNT){
                break;
            }
            if(!listCompare.contains(it->first)){
                list.emplace_back(&it->second);
            }
        }
    }

private:
    std::unordered_map<QString,T> cache;
    std::mutex cacheMutex;
    //std::map<QString,T> cache;
};

#endif // CACHE_H
