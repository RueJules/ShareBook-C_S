/*Auther:GongYanxian
Date:2023.6.19*/
#include "noteBroker.h"
#include <QJsonObject>
#include <QByteArray>
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
        //在服务端新建note实例
        Note note(noteId,title,content,time,materials,netizenId);
        //读素材列表
        QStringList uuidlist=noteObject["materials"].toObject().keys();
         QJsonObject materialsJson=noteObject["materials"].toObject();
        for(const auto &uuid:uuidlist){
            QJsonObject oneMaterial=materialsJson[uuid].toObject();

            //保存图片到服务器目录
            QString imagebaseb4Data= oneMaterial.value("image").toString();
            QByteArray imageData = QByteArray::fromBase64(imagebaseb4Data.toUtf8());
            QPixmap pi;
            pi.loadFromData(imageData);
            pi.save("/root/sharebook/materials" + oneMaterial["path"].toString());

            MaterialProxy mp(uuid);
            note.addMaterial(oneMaterial["order"].toInt(),std::move(mp));
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

QByteArray NoteBroker::getNotes(QString netizenId)
{
    QList<Note*> list;
    m_cache.getNotes(netizenId, list);
    if(list.length() < FIND_COUNT){
        //数据库里在找剩下的几条
        int differrence = FIND_COUNT - list.length();
        std::string cmd = "select * from note where blogger!=\"" + netizenId.toStdString() + "\" LIMIT " + std::to_string(differrence);

        qDebug() << "sql语句！！！！！！！！！！！1"<<cmd <<'\n';

        sql::ResultSet *notesDiff = query(cmd);

        while(notesDiff->next()){
            QString noteId = QString::fromStdString(notesDiff->getString(1).c_str());
            QString title=QString::fromStdString(notesDiff->getString(2).c_str());
            QString content=QString::fromStdString(notesDiff->getString(3).c_str());
            int materials=notesDiff->getInt(4);

            std::string t = notesDiff->getString("time").c_str();
            //qDebug() << "数据库读出来的时间！！！！！！！！！！！！！" << t <<'\n';
            QDateTime time=QDateTime::fromString(QString::fromStdString(t),"yyyy-MM-dd hh:mm:ss");

            QString bloggerId=QString::fromStdString(notesDiff->getString(6).c_str());

            Note note(noteId, title, content,time,materials,bloggerId);

           // qDebug() << "时间！！！！！！！！！！！！！！！！"<<time.toString("yyyy-MM-dd hh:mm:ss") <<Qt::endl;

            std::string cmd_m = "select id,number from material where note_id=\""+noteId.toStdString()+"\"";



            sql::ResultSet *set_m = query(cmd_m);
            while(set_m->next()){
                QString id = QString::fromStdString(set_m->getString(1).c_str());
                int order = set_m->getInt("number");
                MaterialProxy mp(id);
                note.addMaterial(order, std::move(mp));
            }
            //list.emplace_back(&note);
            //加到缓存
            m_cache.addToCache(noteId,std::move(note));
            list.emplace_back(&m_cache.getFromCache(noteId));
        }

    }

    //QJsonObject notesJson;
    //QJsonDocument notesDoc;
    QByteArray arraies;
    for(Note* note:list){
        QJsonDocument oneD;
        QJsonObject notesJson;
        QJsonObject nJson = note->getNoteAbstract();
        nJson.insert("function", "view");

        notesJson.insert(note->get_Id(), QJsonValue(nJson));
        //notesJson.insert("function", "view");

        oneD.setObject(notesJson);
        QByteArray array = oneD.toJson();
        array.append('\r');
        arraies.append(array);
    }
//    notesDoc.fromJson(arraies);
//    qDebug() << arraies;
//    return notesDoc;
    return arraies;
}

void NoteBroker::sycn()
{

}


