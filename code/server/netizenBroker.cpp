/*
 * datetime:2023-6-17
 * author:chenzexi
*/

#include <mutex>
#include <QJsonObject>
#include <QDebug>
#include "netizenBroker.h"
#include "netizen.h"
#include "netizenProxy.h"
#include "noteProxy.h"

std::shared_ptr<NetizenBroker> NetizenBroker::s_netizenBroker = nullptr;
std::mutex NetizenBroker::netizenBrokerMutex;

NetizenBroker::NetizenBroker()
{

}

NetizenBroker::~NetizenBroker()
{

}

std::shared_ptr<NetizenBroker> NetizenBroker::getInstance() {
    //这里使用了两个 if 判断语句进行双检锁
    if (s_netizenBroker == nullptr) {
        std::unique_lock<std::mutex> lock(netizenBrokerMutex);
        if (s_netizenBroker == nullptr) {
            //确保netizenbroker先初始化再让指针指向初始化的位置？？
            auto temp = std::shared_ptr<NetizenBroker>(new NetizenBroker());
            s_netizenBroker = temp;
        }
    }
    return s_netizenBroker;
}

QJsonObject NetizenBroker::matchLoginInfo(QString nickname, QString password)
{
    //匹配id和密码是否正确,获取结果集, 匹配成功就进行初始化
    std::string cmd="select * from netizen where nickname=\""+nickname.toStdString()+"\" and password=\""+password.toStdString()+"\"";
    //qDebug() << cmd << '\n';
    sql::ResultSet *resultSet =query(cmd);

    QJsonObject netizenInfo;

    //判断是否匹配到了账号
    if (resultSet->next()){
        //匹配成功

        QString netizenId=QString::fromStdString(resultSet->getString(1).c_str());
        QString profile=QString::fromStdString(resultSet->getString(4).c_str());//传输时转成二进制

        int concerns=resultSet->getInt(5);
        int fans=resultSet->getInt(6);
        int notes=resultSet->getInt(8);

        //给客户端返回信息
        netizenInfo={
            {"netizenId",netizenId},
            {"password",password},
            {"nickname",nickname},
            {"profile",profile},
            {"concernCount",concerns},
            {"fanCount",fans},
            {"noteCount",notes}
        };
    }
    else{
        netizenInfo={};
    }

    qDebug() << netizenInfo;
    return netizenInfo;
}

void NetizenBroker::createNetizen(QJsonObject netizenObject)
{

    QString netizenId=netizenObject["netizenId"].toString();
    QString nickname=netizenObject["nickname"].toString();
    QString profile=netizenObject["profile"].toString();
    QString password = netizenObject["password"].toString();
    int concerns=netizenObject["concernCount"].toInt();
    int fans=netizenObject["fanCount"].toInt();
    int notes=netizenObject["noteCount"].toInt();
    //在服务端新建netizen实例
    Netizen netizen(netizenId,password,nickname,profile);
    //读关注列表
    if(concerns){
        std::string cmd="select concern_id from netizen_concerns where netizen_id=\""+netizenId.toStdString()+"\"";
        sql::ResultSet *concern_set =query(cmd);
        while(concern_set->next()){
            QString concern=QString::fromStdString(concern_set->getString(1).c_str());
            NetizenProxy np(concern);
            netizen.addConcern(concern,std::move(np));
        }
    }
    //读粉丝列表
    if(fans){
        std::string cmd="select fan_id from netizen_fans where netizen_id=\""+netizenId.toStdString()+"\"";
        sql::ResultSet *fan_set =query(cmd);
        while(fan_set->next()){
            QString fan=QString::fromStdString(fan_set->getString(1).c_str());
            NetizenProxy np(fan);
            netizen.addFan(fan,std::move(np));
        }
    }
    //读发布笔记的列表
    if(notes){
        std::string cmd="select note_id from published_note where blogger_id=\""+netizenId.toStdString()+"\"";
        sql::ResultSet *note_set =query(cmd);
        while(note_set->next()){
            QString note=QString::fromStdString(note_set->getString(1).c_str());
            NoteProxy np(note);
            netizen.addNote(note,std::move(np));
        }
    }
    //加入缓存
    new_cache.addToCache(netizenId,std::move(netizen));

}

Netizen *NetizenBroker::findById(QString netizenId)
{
    if(oldClean_cache.inCache(netizenId)){

        return &oldClean_cache.getFromCache(netizenId);
    }
    else{
        if(new_cache.inCache(netizenId))
        {
            return &new_cache.getFromCache(netizenId);
        }else{
            if(oldDirty_cache.inCache(netizenId))
            {
                return &oldDirty_cache.getFromCache(netizenId);
            }else{
                //从数据库中找
                qDebug() << "netizen cache拦截失败！！！";
//                std::string cmd="select * from netizen where id=\""+netizenId.toStdString()+"\"";
//                sql::ResultSet *set =query(cmd);
//                if(set->next()){
//                    QString password = QString::fromStdString(set->getString(2).c_str());
//                    QString nickname=QString::fromStdString(set->getString(3).c_str());
//                    QString profile=QString::fromStdString(set->getString(4).c_str());
//                    int concerns=set->getInt(5);
//                    int fans=set->getInt(6);
//                    int notes=set->getInt(8);
//                    //在服务端新建netizen实例
//                    Netizen netizen(netizenId, password,nickname, profile);
//                    //读关注列表
//                    if(concerns){
//                        std::string cmd="select concern_id from netizen_concerns where netizen_id=\""+netizenId.toStdString()+"\"";
//                        sql::ResultSet *concern_set =query(cmd);
//                        while(concern_set->next()){
//                            QString concern=QString::fromStdString(concern_set->getString(1).c_str());
//                            NetizenProxy np(concern);
//                            netizen.addConcern(concern,std::move(np));
//                        }
//                    }
//                    //读粉丝列表
//                    if(fans){
//                        std::string cmd="select fan_id from netizen_fans where netizen_id=\""+netizenId.toStdString()+"\"";
//                        sql::ResultSet *fan_set =query(cmd);
//                        while(fan_set->next()){
//                            QString fan=QString::fromStdString(fan_set->getString(1).c_str());
//                            NetizenProxy np(fan);
//                            netizen.addFan(fan,std::move(np));
//                        }
//                    }
//                    //读发布笔记的列表
//                    if(notes){
//                        std::string cmd="select note_id from published_note where blogger_id=\""+netizenId.toStdString()+"\"";
//                        sql::ResultSet *note_set =query(cmd);
//                        while(note_set->next()){
//                            QString note=QString::fromStdString(note_set->getString(1).c_str());
//                            NoteProxy np(note);
//                            netizen.addNote(note,std::move(np));
//                        }
//                    }
//                    //加到缓存
//                    new_cache.addToCache(netizenId,std::move(netizen));
//                    return &new_cache.getFromCache(netizenId);
//                }
            }
        }
    }
    return nullptr;
}

void NetizenBroker::initCache(){
    //涉及的表有：netizen，published_note，netizen_concerns，netizen_fans
    //从数据库中找
    std::unique_lock<std::mutex> lock(netizenBrokerMutex);
    std::string cmd="select * from netizen ";
    sql::ResultSet *set =query(cmd);
    while(set->next()){
        QString netizenId=QString::fromStdString(set->getString(1).c_str());
        QString password=QString::fromStdString(set->getString(2).c_str());
        QString nickname=QString::fromStdString(set->getString(3).c_str());
        QString profile=QString::fromStdString(set->getString(4).c_str());
        int concerns=set->getInt(5);
        int fans=set->getInt(6);
        int notes=set->getInt(8);
        //在服务端新建netizen实例
        Netizen netizen(netizenId,password, nickname, profile);
        //读关注列表
        if(concerns){
            std::string cmd="select concern_id from netizen_concerns where netizen_id=\""+netizenId.toStdString()+"\"";
            sql::ResultSet *concern_set =query(cmd);
            while(concern_set->next()){
                QString concern=QString::fromStdString(concern_set->getString(1).c_str());
                NetizenProxy np(concern);
                netizen.addConcern(concern,std::move(np));
            }
        }
        //读粉丝列表
        if(fans){
            std::string cmd="select fan_id from netizen_fans where netizen_id=\""+netizenId.toStdString()+"\"";
            sql::ResultSet *fan_set =query(cmd);
            while(fan_set->next()){
                QString fan=QString::fromStdString(fan_set->getString(1).c_str());
                NetizenProxy np(fan);
                netizen.addFan(fan,std::move(np));
            }
        }
        //读发布笔记的列表
        if(notes){
            std::string cmd="select note_id from published_note where blogger_id=\""+netizenId.toStdString()+"\"";
            sql::ResultSet *note_set =query(cmd);
            while(note_set->next()){
                QString note=QString::fromStdString(note_set->getString(1).c_str());
                NoteProxy np(note);
                netizen.addNote(note,std::move(np));
            }
        }
        //加到缓存
        oldClean_cache.addToCache(netizenId,std::move(netizen));
    }
    qDebug() << "netizen init finish!\n";

}
void NetizenBroker::sycn()
{
    //和netizen、published_note、checked_note表相关的操作
    //将new_cache中新创建的（创建的，创建后没写入数据库就修改的）数据更新到数据库
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(FRESH_TIME));
        std::lock_guard<std::mutex> lock(netizenBrokerMutex);
        std::unordered_map<QString,Netizen>& nc=new_cache.getCacheConst();
        for(auto it = nc.begin(); it != nc.end(); ++it){
            QString netizenId;
            QString password;
            QString nickname;
            QString profile;
            QList<QString> concerns;
            QList<QString> fans;
            QList<QString> notes;
            QList<QString> footmark;
            Netizen &netizen=it->second;
            netizen.getInfo(netizenId,password,nickname,profile);
            netizen.getConcernList(concerns);
            netizen.getFanList(fans);
            netizen.getNoteList(notes);
            netizen.getFootmarkList(footmark);
            std::string cmd="insert ignore into netizen(id,password,nickname,profile_photo,concerns,fans,notes) value(\""+netizenId.toStdString()+"\",\""+password.toStdString()+"\",\""+nickname.toStdString()+"\",\""+profile.toStdString()+"\","+std::to_string(concerns.size())+","+std::to_string(fans.size())+","+std::to_string(notes.size())+")";
            insert(cmd);
            for(auto netizen:concerns){
                QString id=netizenId+netizen;
                std::string cmd="insert ignore into netizen_concerns(id,netizen_id,concern_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+netizen.toStdString()+"\")";
                qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto netizen:fans){
                QString id=netizenId+netizen;
                std::string cmd="insert ignore into netizen_fans(id,netizen_id,fan_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+netizen.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto note:footmark){
                QString id=netizenId+note;
                std::string cmd="insert ignore into checked_note(id,netizen_id,note_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+note.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto note:notes){
                QString id=netizenId+note;
                std::string cmd="insert ignore into published_note(id,blogger_id,note_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+note.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
        }
        new_cache.clearCache();
        //将oldDirty_cache中被修改的（数据库中原本存在但现在被修改）数据更新到数据库
        std::unordered_map<QString,Netizen>& odirtyc=oldDirty_cache.getCacheConst();
        for(auto it = odirtyc.begin(); it != odirtyc.end(); ++it){
            QString netizenId;
            QString password;
            QString nickname;
            QString profile;
            QList<QString> concerns;
            QList<QString> fans;
            QList<QString> notes;
            QList<QString> footmark;
            Netizen &netizen=it->second;
            netizen.getInfo(netizenId,password,nickname,profile);
            netizen.getConcernList(concerns);
            netizen.getFanList(fans);
            netizen.getNoteList(notes);
            netizen.getFootmarkList(footmark);
            std::string cmd="update netizen set password=\""+password.toStdString()+"\",nickname=\""+nickname.toStdString()+"\",profile_photo=\""+profile.toStdString()+"\",concerns="+std::to_string(concerns.size())+",fans="+std::to_string(fans.size())+",notes="+std::to_string(notes.size())+" where id=\""+netizenId.toStdString()+"\"";
            update(cmd);
            for(auto netizen:concerns){
                QString id=netizenId+netizen;
                std::string cmd="insert ignore into netizen_concerns(id,netizen_id,concern_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+netizen.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto netizen:fans){
                QString id=netizenId+netizen;
                std::string cmd="insert ignore into netizen_fans(id,netizen_id,fan_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+netizen.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto note:footmark){
                QString id=netizenId+note;
                std::string cmd="insert ignore into checked_note(id,netizen_id,note_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+note.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
            for(auto note:notes){
                QString id=netizenId+note;
                std::string cmd="insert ignore into published_note(id,blogger_id,note_id) value(\""+id.toStdString()+"\",\""+netizenId.toStdString()+"\",\""+note.toStdString()+"\")";
                        qDebug() << cmd << '\n';
                insert(cmd);
            }
        }
        oldDirty_cache.clearCache();
        //将oldDelete_cache中被删除的（数据库中原本存在但现在被删除）数据更新到数据库
        std::unordered_map<QString,Netizen>& odeletec=oldDelete_cache.getCacheConst();
        for(auto it = odeletec.begin(); it != odeletec.end(); ++it){
            Netizen &netizen=it->second;
            QString netizenId=netizen.id();
            std::string cmd="delete from netizen where id=\""+netizenId.toStdString()+"\"";
                    qDebug() << cmd << '\n';
            drop(cmd);
        }
        oldDelete_cache.clearCache();
        //更新oldClean_cache
        initCache();
    }

}

void NetizenBroker::start_thread()
{
    m_sycn_thread=new std::thread(&NetizenBroker::sycn,this);
}
