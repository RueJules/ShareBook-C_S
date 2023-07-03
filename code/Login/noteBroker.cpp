/*Auther:GongYanxian
Date:2023.6.19*/
#include "noteBroker.h"
#include <QJsonObject>
#include<iostream>
std::shared_ptr<NoteBroker> NoteBroker::s_noteBroker = nullptr;
std::mutex NoteBroker::noteBrokerMutex;
NoteBroker::NoteBroker()
{

}
NoteBroker::~NoteBroker()
{

}

std::shared_ptr<NoteBroker> NoteBroker::getInstance()
{
    if(s_noteBroker == nullptr){
        std::unique_lock<std::mutex> lock(noteBrokerMutex);
        if(s_noteBroker==nullptr){
            auto temp = std::shared_ptr<NoteBroker>(new NoteBroker());
            s_noteBroker = temp;
        }
    }
    return s_noteBroker;

}

Note *NoteBroker::findById(QString noteId)
{
    if(m_cache.inCache(noteId)){
        return &m_cache.getFromCache(noteId);
    }
    else{
        //从数据库中找
        std::string cmd="select * from note where id=\""+noteId.toStdString()+"\"";
        sql::ResultSet *set =query(cmd);
        QString title=QString::fromStdString(set->getString(2).c_str());
        QString content=QString::fromStdString(set->getString(3).c_str());
        int materials=set->getInt(4);
        QDateTime time=QDateTime::fromString(QString::fromStdString(set->getString(5).c_str()),"yyyyMMddhhmmss");
        QString bloggerId=QString::fromStdString(set->getString(6).c_str());


        //初始化素材容器，只获取所有素材的id和在笔记中的order,不生成实例
        std::string cmd_m = "select id,number from material where note_id=\""+noteId.toStdString()+"\"";

        Note note(noteId, title, content,time,materials,bloggerId);

        sql::ResultSet *set_m = query(cmd_m);
        while(set_m->next()){
            QString id = QString::fromStdString(set_m->getString(1).c_str());
            int order = set_m->getInt("number");
            MaterialProxy mp(id);
            note.addMaterial(order, std::move(mp));
        }

        //加到缓存
        m_cache.addToCache(noteId,std::move(note));
        return &m_cache.getFromCache(noteId);
    }
    return nullptr;
}

void NoteBroker::createNote(QJsonObject noteObject)
{
    if(noteObject.isEmpty()){
        QString noteId=noteObject["noteId"].toString();
        QString title=noteObject["title"].toString();
        QString content=noteObject["content"].toString();
        QString netizenId=noteObject["netizenId"].toString();
        int materials=noteObject["materialCount"].toInt();
        QDateTime time=QDateTime::currentDateTime();
        //在服务端新建netizen实例
        Note note(noteId,title,content,time,materials,netizenId);
        //读素材列表
        QStringList uuidlist=noteObject["materials"].toObject().keys();
        for(const auto &uuid:uuidlist){
            QJsonObject o=noteObject["materials"].toObject();
            QJsonObject o1=o[uuid].toObject();
            MaterialProxy mp(uuid);
            note.addMaterial(o1["order"].toInt(),std::move(mp));
        }
        //加入缓存
        m_cache.addToCache(noteId,std::move(note));
    }
}

void NoteBroker::initCache()
{
    //涉及的表：note,
    //material列表都设置为空的proxy
}

QJsonDocument NoteBroker::getNotes(std::string netizenId)
{
//    QList<Note> list;
//    m_cache.getNotes(netizenId, list);
//    QJsonObject noteJson;
//    for(auto note:list){

//    }
    //一次查找10条笔记，不包括自己ID的笔记
    //现在cache中查找

}

void NoteBroker::sycn()
{

}


