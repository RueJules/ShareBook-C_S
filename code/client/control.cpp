#include "control.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QList>

#define THREAD_COUNT 3

boost::asio::io_service Control::m_service;

Control::Control():work{new boost::asio::io_service::work(m_service)}
{
    connect_socket=std::make_shared<Client>(m_service,"10.252.55.175",2001,this);
    for ( int i = 0; i < THREAD_COUNT; ++i)
        threads.create_thread(boost::bind(&boost::asio::io_service::run, &m_service));

}
void Control::requestLogin(QString nickname, QString psw)
{
    QJsonObject loginInfo;
    loginInfo={
        {"function","login"},
        {"nickname",nickname},
        {"password",psw}
    };
    QJsonDocument doc(loginInfo);
    QByteArray data=doc.toJson();
    connect_socket->do_write(data);
}

void Control::receiveLoginInfo(QByteArray data)
{
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
    if(!obj.isEmpty()){
        netizenId=obj["netizenId"].toString();
        //还有读头像和关注粉丝数量，有了推荐算法后这里还可以读喜好
        //应该把这些基本信息写入客户端本地的配置文件
       // qDebug()<<obj;
        emit getAccountInfo(true);
        qDebug()<<"发了一个信号";
    }
    else{
        emit getAccountInfo(false);
        qDebug()<<"发了两个信号";
    }

}

void Control::requestNotes()
{
    QJsonObject netizenInfo;//只传id,有推荐算法后也可以传他的喜好信息
    netizenInfo={
        {"function","view"},
        {"netizenId",netizenId},
    };
    QJsonDocument doc(netizenInfo);
    QByteArray data=doc.toJson();
    connect_socket->do_write(data);
    qDebug()<<"获取更多笔记";
}
void Control::receiveNotes(QByteArray data)
{
    //循环遍历每一个笔记
    //把他转换Model的list来源
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
    QStringList keys=obj.keys();
    QList<QList<QVariant>> sl;
    for(const auto &key:keys){
        QList<QVariant> temp;//noteId
        temp.append(key);
        auto value=obj.value(key).toObject();
        temp.append(value.value("time").toVariant());
        temp.append(value.value("title").toVariant());
        temp.append(value.value("FirstImg").toVariant());
        auto blogger=value.value("Blogger").toObject();
        temp.append(blogger.value("nickname").toVariant());
        temp.append(blogger.value("profile").toVariant());
        sl.append(temp);
    }
    emit getNewNotes(sl);
}

void Control::requestNoteDetail(QString noteId)
{
    QJsonObject noteInfo;
    noteInfo={
        {"function","check"},
        {"noteId",noteId}
    };
    QJsonDocument doc(noteInfo);
    QByteArray data=doc.toJson();
    connect_socket->do_write(data);
}

void Control::receiveNoteDetail(QByteArray data)
{
    //循环遍历每一个笔记
    //把他转换Model的list来源
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
    QStringList keys=obj.keys();
    QList<QList<QVariant>> sl;
    for(const auto &key:keys){
        QList<QVariant> temp;
        temp.append(key);
        auto value=obj.value(key).toObject();
        QStringList ks=value.keys();
        for(const auto &k:ks){
            auto v=obj.value(key).toVariant();
            temp.append(v);
        }
        sl.append(temp);
    }
    emit getNoteDetail(sl);
}

void Control::requestCommentDetail(QString noteId)
{
    QJsonObject commentInfo;
    commentInfo={
        {"function","checkComment"},
        {"noteId",noteId}
    };
    QJsonDocument doc(commentInfo);
    QByteArray data=doc.toJson();
    connect_socket->do_write(data);
}

void Control::receiveCommentDetail(QByteArray data)
{

}

void Control::requestReplyDetail(QString commentId)
{
    QJsonObject replyInfo;
    replyInfo={
        {"function","checkReply"},
        {"commentId",commentId}
    };
    QJsonDocument doc(replyInfo);
    QByteArray data=doc.toJson();
    connect_socket->do_write(data);
}

void Control::receiveReplyDetail(QByteArray data)
{

}
