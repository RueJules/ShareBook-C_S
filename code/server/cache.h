#ifndef CACHE_H
#define CACHE_H
#include <QString>
#include <memory>
#include <QList>
#include <unordered_map>
#include <QDebug>

#define FIND_COUNT 3

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
        return cache.at(id);  //这里是不是应该用弹出？不然如果涉及修改，就不能把新修改的加入进去了
    }
    void addToCache(QString id,T &&entity){
        //question:加入缓存中的数据无法读取。
        qDebug() << "cachesize:"<<cache.size() << '\n';
        cache.emplace_hint(cache.begin(),id,std::move(entity));
    }
    void  getSome(QList<QString> &listCompare, QList<T*> &list){

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
    //std::map<QString,T> cache;
};

#endif // CACHE_H
