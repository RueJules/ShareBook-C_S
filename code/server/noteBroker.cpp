/*Auther:GongYanxian
Date:2023.6.19*/
#include "noteBroker.h"
#include <QJsonObject>
#include <QByteArray>
#include "netizen.h"
#include "netizenBroker.h"
#include<iostream>
#include  "materialProxy.h"

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
    if(old_clean_cache.inCache(noteId)){

        return &old_clean_cache.getFromCache(noteId);
    }
    else{
        if(newCache.inCache(noteId))
        {
            return &newCache.getFromCache(noteId);

        }else{

            if(old_dirty_cache.inCache(noteId))
            {
                return &old_dirty_cache.getFromCache(noteId);

            }else{

                //从数据库中找
                qDebug() << "noteCache 拦截失败！\n";
//                std::string cmd="select * from note where id=\""+noteId.toStdString()+"\"";
//                sql::ResultSet *set =query(cmd);
//                while(set->next())
//                {
//                    QString title=QString::fromStdString(set->getString(2).c_str());
//                    QString content=QString::fromStdString(set->getString(3).c_str());
//                    int materials=set->getInt(4);
//                    QDateTime time=QDateTime::fromString(QString::fromStdString(set->getString(5).c_str()),"yyyyMMddhhmmss");
//                    QString bloggerId=QString::fromStdString(set->getString(6).c_str());


//                    Note note(noteId, title, content,time,materials,bloggerId);

//                    //初始化素材容器，只获取所有素材的id和在笔记中的order,不生成实例
//                    std::string cmd_m = "select id,number from material where note_id=\""+noteId.toStdString()+"\"";
//                    sql::ResultSet *set_m = query(cmd_m);
//                    while(set_m->next()){
//                        QString id = QString::fromStdString(set_m->getString(1).c_str());
//                        int order = set_m->getInt("number");
//                        MaterialProxy mp(id);
//                        note.addMaterial(order, std::move(mp));
//                    }

//                    //加到缓存
//                    newCache.addToCache(noteId,std::move(note));
//                    return &newCache.getFromCache(noteId);
//                }
            }
        }
    }
    return nullptr;
}

bool NoteBroker::createNote(QJsonObject noteObject)
{
    if(!noteObject.isEmpty()){
        QString noteId=noteObject["noteId"].toString();
        QString title=noteObject["title"].toString();
        QString content=noteObject["content"].toString();
        QString netizenId=noteObject["netizenId"].toString();
        int materials=noteObject["materialCount"].toInt();
        QDateTime time=QDateTime::currentDateTime();
        //在服务端新建note实例
        Note note(noteId,title,content,time,materials,netizenId);
        //读素材列表
        QStringList uuidlist=noteObject["materials"].toObject().keys();
         QJsonObject materialsJson=noteObject["materials"].toObject();
        for(const auto &uuid:uuidlist){
            QJsonObject oneMaterial=materialsJson[uuid].toObject();
            MaterialProxy mp(uuid);
            note.addMaterial(oneMaterial["order"].toInt(),std::move(mp));
        }
        //加入缓存
        newCache.addToCache(noteId,std::move(note));

        //判断是否加入成功
        bool yes_ = newCache.inCache(noteId) ? true : false;
        qDebug() << "-------------" << yes_ << Qt::endl;
        return yes_;

    }else{
        return false;
    }

}

void NoteBroker::initCache()
{
    //涉及的表：note,material
    //从数据库中读取所有发布的笔记（id,title,content,time,materials,blogger）
    std::string cmd_n = "select *from note";
    sql::ResultSet *res_n = query(cmd_n);
    while(res_n->next())
    {
        //发布的笔记（id,title,content,time,materials,blogger）
        QString noteId = QString::fromStdString(res_n->getString(1).c_str());
        QString title = QString::fromStdString(res_n->getString(2).c_str());
        QString content = QString::fromStdString(res_n->getString(3).c_str());
        int materials = res_n->getInt(4);
        std::string t = res_n->getString("time").c_str();
        QDateTime time=QDateTime::fromString(QString::fromStdString(t),"yyyy-MM-dd hh:mm:ss");
        QString bloggerId=QString::fromStdString(res_n->getString(6).c_str());
        Note note(noteId, title, content,time,materials,bloggerId);

        std::string cmd_m = "select id,number from material where note_id=\""+noteId.toStdString()+"\"";
        sql::ResultSet *set_m = query(cmd_m);
        //初始化笔记关联的materialproxy（material ids）
        while(set_m->next())
        {
            QString id = QString::fromStdString(set_m->getString(1).c_str());
            int order = set_m->getInt("number");
            MaterialProxy mp(id);
            note.addMaterial(order, std::move(mp));
        }
        old_clean_cache.addToCache(noteId, std::move(note));
    }
    qDebug() << "note init finish!\n";
    //把note加入cache
}

QByteArray NoteBroker::getNotes(QString netizenId)
{
    QList<Note*> list;
    Netizen *netizen = NetizenBroker::getInstance()->findById(netizenId);
    QList<QString> noteList;
    netizen->getNoteList(noteList);

    //在三个cache里面获取一定数量的笔记
    old_clean_cache.getSome(noteList, list);
    newCache.getSome(noteList,list);
    old_dirty_cache.getSome(noteList, list);

    qDebug() << "length!!!!!!!!!!!!!!!!!!!!"<<list.length() <<'\n';

    //如果在三个缓存里面找到的数量不足，就去数据库里面找，但有可能这里始终也不需要
    if(list.length() < FIND_COUNT){
        //数据库里在找剩下的几条
        int differrence = FIND_COUNT - list.length();
//        std::string cmd = "select * from note where blogger!=\"" + netizenId.toStdString() + "\" LIMIT " + std::to_string(differrence);
        std::string cmd = "select * from note where blogger!=\"" + std::to_string(1) + "\" LIMIT " + std::to_string(differrence);

        qDebug() << "sql语句！！！！！！！！！！！1"<<cmd <<'\n';

        sql::ResultSet *notesDiff = query(cmd);

        while(notesDiff->next()){
            QString noteId = QString::fromStdString(notesDiff->getString(1).c_str());
            QString title=QString::fromStdString(notesDiff->getString(2).c_str());
            QString content=QString::fromStdString(notesDiff->getString(3).c_str());
            int materials=notesDiff->getInt(4);
            std::string t = notesDiff->getString("time").c_str();
            QDateTime time=QDateTime::fromString(QString::fromStdString(t),"yyyy-MM-dd hh:mm:ss");
            QString bloggerId=QString::fromStdString(notesDiff->getString(6).c_str());
            Note note(noteId, title, content,time,materials,bloggerId);

            std::string cmd_m = "select id,number from material where note_id=\""+noteId.toStdString()+"\"";
            sql::ResultSet *set_m = query(cmd_m);
            while(set_m->next()){
                QString id = QString::fromStdString(set_m->getString(1).c_str());
                int order = set_m->getInt("number");
                MaterialProxy mp(id);
                note.addMaterial(order, std::move(mp));
            }

            //加到缓存
            newCache.addToCache(noteId,std::move(note));
            list.emplace_back(&newCache.getFromCache(noteId));
        }

    }

    //QJsonObject notesJson;
    //QJsonDocument notesDoc;
    QByteArray arraies;
    for(Note* note:list){
        QJsonDocument oneD;
        QJsonObject notesJson;
        QJsonObject nJson = note->getNoteAbstract();
        //nJson.insert("function", "view");

        notesJson.insert(note->get_Id(), QJsonValue(nJson));
        notesJson.insert("function", "view");

        oneD.setObject(notesJson);
        QByteArray array = oneD.toJson();
        array.append('\r');
        arraies.append(array);
    }
    return arraies;
}

void NoteBroker::sycn()
{
    std::unordered_map<QString,Note> &mapFromNewCache = newCache.getCacheConst();
    std::unordered_map<QString,Note> &mapFromODCache = old_dirty_cache.getCacheConst();
    std::unordered_map<QString,Note> &mapFromODECache = old_deleted_cache.getCacheConst();

    //newCache内包括了脏的
    for(auto it = mapFromNewCache.begin();it != mapFromNewCache.end(); ++it)
    {
        //更新note表
        Note &note = it->second;
        QJsonObject oneNote =note.toDB();
        QString id = oneNote["id"].toString();
        QString title = oneNote["title"].toString();
        QString content = oneNote["content"].toString();
        int materials = oneNote["materials"].toInt();
        QString time = oneNote["time"].toString();
        QString blogger = oneNote["blogger"].toString();
        std::string cmd = "insert ignore into note values(\"" + id.toStdString() + "\",\""+title.toStdString() + "\",\"" + content.toStdString() +"\",\"" + std::to_string(materials)+"\",\"" + time.toStdString() + "\",\"" + blogger.toStdString() + "\")";
        insert(cmd);
    }
    for(auto it = mapFromODCache.begin();it != mapFromODCache.end(); ++it)
    {
        //更新note表
        Note &note = it->second;
        QJsonObject oneNote =note.toDB();
        QString id = oneNote["id"].toString();
        QString title = oneNote["title"].toString();
        QString content = oneNote["content"].toString();
        int materials = oneNote["materials"].toInt();
        QString time = oneNote["time"].toString();
        QString blogger = oneNote["blogger"].toString();
        std::string cmd = "update note set title=\""+title.toStdString() + "\",set content=\"" + content.toStdString() +"\",set materials=" + std::to_string(materials)+",set time=\"" +        time.toStdString() + "\",set blogger=\"" + blogger.toStdString()+ "\"where id=\"" + id.toStdString() + "\"";
        update(cmd);
    }
    for(auto it = mapFromODECache.begin();it != mapFromODECache.end(); ++it)
    {
        //更新note表
        Note &note = it->second;
        QJsonObject oneNote =note.toDB();
        QString id = oneNote["id"].toString();
        std::string cmd = "delete from note where id=\"" + id.toStdString() + "\"";
        drop(cmd);
    }
    //同步完成，再次初始化old_clean_cache
    initCache();
}


