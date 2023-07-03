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

        qDebug() << "ookokoko\n";
        QString netizenId=QString::fromStdString(resultSet->getString(1).c_str());
        QString profile=QString::fromStdString(resultSet->getString(4).c_str());//传输时转成二进制

        qDebug() << "ookokoko\n";

        int concerns=resultSet->getInt(5);
        int fans=resultSet->getInt(6);
        int notes=resultSet->getInt(8);

        //给客户端返回信息
        netizenInfo={
            {"function", "login"},
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
    return netizenInfo;
}

void NetizenBroker::createNetizen(QJsonObject netizenObject)
{

    QString netizenId=netizenObject["netizenId"].toString();
    QString nickname=netizenObject["nickname"].toString();
    QString profile=netizenObject["profile"].toString();
    int concerns=netizenObject["concernCount"].toInt();
    int fans=netizenObject["fanCount"].toInt();
    int notes=netizenObject["noteCount"].toInt();
    //在服务端新建netizen实例
    Netizen netizen(netizenId,nickname,profile);
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
    m_cache.addToCache(netizenId,std::move(netizen));

}

Netizen *NetizenBroker::findById(QString netizenId)
{
    if(m_cache.inCache(netizenId)){
        return &m_cache.getFromCache(netizenId);
    }
    else{
        //从数据库中找
        std::string cmd="select * from netizen where id=\""+netizenId.toStdString()+"\"";
        sql::ResultSet *set =query(cmd);
        QString nickname=QString::fromStdString(set->getString(3).c_str());
        QString profile=QString::fromStdString(set->getString(4).c_str());
        int concerns=set->getInt(5);
        int fans=set->getInt(6);
        int notes=set->getInt(8);
        //在服务端新建netizen实例
        Netizen netizen(netizenId, nickname, profile);
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
        m_cache.addToCache(netizenId,std::move(netizen));
        return &m_cache.getFromCache(netizenId);
    }
    return nullptr;
}
void NetizenBroker::initCache(){
    //涉及的表有：netizen，published_note，netizen_concerns，netizen_fans

}
void NetizenBroker::sycn()
{
    //每隔多少多少秒就自动更新一次这个broker对应的数据库中的表
//    QTimer timer;
//    QObject::connect(&timer, &QTimer::timeout, [&cache]() {
//        cache.sync();
//    });
//    timer.start(10000);

    //用INSERT IGNORE在插入值相同时（也就是没有更新）忽略这一次插入
    // 创建一个SQL语句对象
    //    sql::Statement* stmt = conn->createStatement();

    //    // 遍历自定义缓存中的所有数据项，并将其同步到数据库中
    //    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
    //        int key = it->first;
    //        std::string value = it->second;

    //        // 执行INSERT语句，将数据插入到数据库中
    //        std::string sqlStr = "INSERT INTO mytable (key, value) VALUES (" + std::to_string(key) + ", '" + value + "')";
    //        stmt->executeUpdate(sqlStr);
    //    }
}
