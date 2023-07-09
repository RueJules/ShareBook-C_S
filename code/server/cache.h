#ifndef CACHE_H
#define CACHE_H
#include <QString>
#include <memory>
#include <QList>

#define FIND_COUNT 1

template<typename T>
class Cache
{
public:
    Cache() = default;
    explicit Cache(const std::unordered_map<QString,T> c) : cache(c) {}
    bool inCache(QString id){
        if(cache.count(id))
            return true;
        else
            return false;
    }
    T& getFromCache(QString id){
        return cache.at(id);
    }
    void addToCache(QString id,T &&entity){
        cache.emplace(id,std::move(entity));
    }
    void  getNotes(QString netizenID, QList<T*> &list){
        for(auto it = cache.begin(); it != cache.end(); ++it){
            if(list.size() >= FIND_COUNT){
                break;
            }
            if(it->first != netizenID){
                list.emplace_back(&it->second);
            }
        }
    }

private:
    std::unordered_map<QString,T> cache;
};

#endif // CACHE_H
