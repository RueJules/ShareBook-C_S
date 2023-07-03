/*
 * datetime:2023-6-17
 * author:chenzexi
*/
#include <memory>
#include <QJsonObject>
#include <QJsonDocument>
#include <QBuffer>
#include <QDebug>
#include "control.h"
#include "netizen.h"
#include "netizenBroker.h"
#include "materialBroker.h"
#include "noteBroker.h"
#include "noteProxy.h"
QJsonDocument Control::matchLoginInfo(QByteArray data)
{
    //qDebug() << data;
    QJsonDocument loadDoc=QJsonDocument::fromJson(data);
    QJsonObject accountInfo=loadDoc.object();
    qDebug() << accountInfo;
    QString nickname=accountInfo["nickname"].toString();
    QString password=accountInfo["password"].toString();
    QJsonObject netizenInfo=NetizenBroker::getInstance()->matchLoginInfo(nickname,password);
    if(!netizenInfo.isEmpty()){
        //给图片转码
        QImage image(netizenInfo["profile"].toString());
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer,"png",100);
        buffer.close(); // 关闭QBuffer

        netizenInfo["profile"]=QJsonValue::fromVariant(imageData);

        //先发送给客户端
        QJsonDocument account(netizenInfo);
        //调用socket写
        account.toJson().constData();

        //再在服务器这边创建netizen实例
        NetizenBroker::getInstance()->createNetizen(netizenInfo);
        return account;
    }
    QJsonDocument account;
    return account;
}

void Control::dealRequestPublishNote(QByteArray data)
{
    QJsonDocument loadDoc=QJsonDocument::fromJson(data);
    QJsonObject noteDetailInfo=loadDoc.object();
    if(!noteDetailInfo.isEmpty()){
        //在服务器这边创建material实例
        MaterialBroker::getInstance()->createMaterial(noteDetailInfo["materails"].toObject());
        //在服务器这边创建note实例
        NoteBroker::getInstance()->createNote(noteDetailInfo);
        //把note添加到netizen的发布笔记列表
        Netizen *blogger=NetizenBroker::getInstance()->findById(noteDetailInfo["netizenId"].toString());
        if(!blogger){
            blogger->addNote(noteDetailInfo["noteId"].toString(),std::move( NoteProxy(noteDetailInfo["noteId"].toString())));
        }
    }
}
