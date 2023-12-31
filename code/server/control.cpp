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
#include "commentbroker.h"

QJsonDocument Control::matchLoginInfo(QByteArray &data)
{
    //qDebug() << data;
    QJsonDocument loadDoc=QJsonDocument::fromJson(data);
    QJsonObject accountInfo=loadDoc.object();

    //qDebug() << accountInfo;
    QString nickname=accountInfo["nickname"].toString();
    QString password=accountInfo["password"].toString();
    QJsonObject netizenInfo=NetizenBroker::getInstance()->matchLoginInfo(nickname,password);


    if(!netizenInfo.isEmpty()){
        //NetizenBroker::getInstance()->createNetizen(netizenInfo);
        //给图片转码
        QImage image(netizenInfo["profile"].toString());
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer,"jpg",1);
        buffer.close(); // 关闭QBuffer

        netizenInfo["profile"]=QJsonValue::fromVariant(imageData);

        //先发送给客户端
        netizenInfo.insert("function", "login");
        QJsonDocument account(netizenInfo);

        //再在服务器这边创建netizen实例

        return account;
    }
    netizenInfo.insert("function", "login");
    QJsonDocument account(netizenInfo);
    return account;
}

QByteArray Control::dealRequestRecommendNote(QByteArray &data)
{
    QJsonDocument loadDoc=QJsonDocument::fromJson(data);
    QJsonObject noteDetailInfo=loadDoc.object();
    QString requestId = noteDetailInfo["netizenId"].toString();
    return NoteBroker::getInstance()->getNotes(requestId);
}

QByteArray Control::dealRequestNoteDetail(QByteArray &data)
{
    //解析出笔记ID
    QJsonDocument detailRequest = QJsonDocument::fromJson(data);
    //detailRequest.fromJson(data);
    QJsonObject j = detailRequest.object();
    QString noteId = j.value("noteId").toString();

    //找到这个笔记实例，并且获得笔记详情需要的笔记内容和所有素材信息
    Note *note = NoteBroker::getInstance()->findById(noteId);
    QJsonObject replyJson = note->getNoteDetails();

    QJsonDocument replyDoc;
    replyDoc.setObject(replyJson);
    QByteArray bytes = replyDoc.toJson();
    bytes.append('\r');
    return bytes;

}

QByteArray Control::dealRequestPublishNote(QByteArray &data)
{
    QJsonDocument loadDoc=QJsonDocument::fromJson(data);
    QJsonObject noteDetailInfo=loadDoc.object();

    QByteArray res;
    QJsonObject publishRes;
    QJsonDocument publishDoc;

    if(!noteDetailInfo.isEmpty()){

        //在服务器这边创建material实例,保存图片到服务器本地
        QJsonObject mJ = noteDetailInfo["materials"].toObject();
        bool materialsCreated = true;
        if(!mJ.isEmpty()){
            materialsCreated = MaterialBroker::getInstance()->createMaterial(noteDetailInfo["noteId"].toString(),mJ);
        }

        //在服务器这边创建note实例
        bool noteCreated = NoteBroker::getInstance()->createNote(noteDetailInfo);

        //把note添加到netizen的发布笔记列表,博主不存在的情况需要判断吗
        Netizen *blogger=NetizenBroker::getInstance()->findById(noteDetailInfo["netizenId"].toString());
        if(blogger){
            blogger->addNote(noteDetailInfo["noteId"].toString(),std::move( NoteProxy(noteDetailInfo["noteId"].toString())));

            //把这个blogger要移入oldDirty_cache
            NetizenBroker::getInstance()->oldClean_To_oldD(blogger->id());
        }

        qDebug() << "material:  "<<materialsCreated <<"note: " <<  noteCreated << Qt::endl;

        if(materialsCreated && noteCreated)
        {
            publishRes = {

                {"function","publish"},
                {"result", "true"}
            };

        }else{

            publishRes = {

                {"function","publish"},
                {"result", "false"}
            };
        }
    }else{
        publishRes = {

            {"function","publish"},
            {"result", "false"}
        };
    }
    publishDoc.setObject(publishRes);
    res = publishDoc.toJson();
    res.append('\r');
    return res;
}

QByteArray Control::dealRequestPublishComment(QByteArray &data)
{
    //处理评论数据
    QJsonDocument Jdoc = QJsonDocument::fromJson(data);
    QJsonObject jObj = Jdoc.object();
    bool commentPublished = false ;
    commentPublished= CommentBroker::getInstance()->createComment(jObj);
    Note *note = NoteBroker::getInstance()->findById(jObj["noteId"].toString());

    note->addComment(jObj["commentId"].toString());

    //返回处理结果
    QJsonObject reObj = {
        {"function","publish_comment"}
    };
    qDebug() << commentPublished;
    if(commentPublished)
    {
        reObj.insert("result","true");

    }else{

        reObj.insert("result","false");
    }
    QJsonDocument reDoc;
    reDoc.setObject(reObj);
    qDebug() << reObj <<'\n';
    QByteArray res = reDoc.toJson();
    res.append('\r');
    return res;

}

QByteArray Control::dealRequestComments(QByteArray &data)
{
    //由于还没有做到回复评论，现在先只寻找一定数量的评论
    QJsonDocument commentDoc = QJsonDocument::fromJson(data);
    QJsonObject commentObj = commentDoc.object();
    qDebug() << commentObj;
    QString noteId = commentObj["noteId"].toString();
    int flag = commentObj["count"].toInt();

    //得到这次要传回去的评论有哪些
    Note *note = NoteBroker::getInstance()->findById(noteId);
    QList<QString> commentIDs ;
    note->commentList(flag, commentIDs);

    qDebug() << commentIDs.size()<<"+++++++++++++++\n";

    //开始获取每条评论的内容
    QJsonObject comments;
    for(auto commentId:commentIDs)
    {
        Comment *comment = CommentBroker::getInstance()->findById(commentId);
        QJsonObject commentObj = comment->getDetails();
        Netizen *owner = NetizenBroker::getInstance()->findById(commentObj["owner_id"].toString());
        QJsonObject ownerInfo = owner->getAbstract();
        commentObj.insert("ownerInfo", QJsonValue(ownerInfo));
        comments.insert(commentId, QJsonValue(commentObj));

    }
    QJsonDocument resDoc;
    comments.insert("function", "check_comment");
    resDoc.setObject(comments);
    qDebug() << comments.keys()<<"keys!!!!!!!!!!!!!!!!\n";
    QByteArray res = resDoc.toJson();
    res.append('\r');
    return res;

}

QByteArray Control::dealRequestViewVideos(QByteArray &data)
{
    //取出笔记中的素材类型是视频的笔记信息

}

