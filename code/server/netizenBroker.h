/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#ifndef NETIZENBROKER_H
#define NETIZENBROKER_H
#include <memory>
#include <string>
#include "netizen.h"
#include "relationalBroker.h"
#include <thread>

class NetizenBroker : public RelationalBroker {

public:
    ~NetizenBroker();
    static std::shared_ptr<NetizenBroker> getInstance();//获取单例
    QJsonObject matchLoginInfo(QString nickname,QString password);
    Netizen *findById(QString netizenId);
    void createNetizen(QJsonObject netizenObject);  
    void initCache();//同步数据库，把数据库的内容写到cache里
    void sycn();//同步数据库，把cache的内容写回数据库
    void sycn_signIn();
    //void start_thread();
    void oldClean_To_oldD(QString netizenID);

private:

    NetizenBroker();
    static std::shared_ptr<NetizenBroker> s_netizenBroker;
    static std::mutex netizenBrokerMutex;
    //std::thread * m_sycn_thread;

    Cache<Netizen>new_cache;
    Cache<Netizen>oldClean_cache;
    Cache<Netizen>oldDirty_cache;
    Cache<Netizen>oldDelete_cache;
};

#endif
