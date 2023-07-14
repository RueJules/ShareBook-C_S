/*Auther:GongYanxian
Date:2023.6.19*/
#ifndef NOTEBROKER_H
#define NOTEBROKER_H
#include "note.h"
#include "relationalBroker.h"
#include <QByteArray>
#include <vector>
#include <mutex>
#include <QJsonDocument>


class NoteBroker : public RelationalBroker {


public:
    ~NoteBroker();
    static std::shared_ptr<NoteBroker> getInstance();//获取单例
    Note *findById(QString noteId);
    bool createNote(QJsonObject noteObject);
    void initCache();//同步数据库，把数据库的内容写到cache里
    QByteArray getNotes(QString netizenId);
    void sycn();//同步数据库，把cache的内容写回数据库
private:
    NoteBroker();
    static std::shared_ptr<NoteBroker> s_noteBroker;
    static std::mutex noteBrokerMutex;
    Cache<Note>m_cache;
};

#endif
