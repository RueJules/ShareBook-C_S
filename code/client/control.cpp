#include "control.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QList>
#include <QPixmap>
#include <QBuffer>
#define THREAD_COUNT 3

boost::asio::io_service Control::m_service;

Control::Control()
    :work{new boost::asio::io_service::work(m_service)}
    ,m_material_provider{new ImageProvider},m_profile_provider{new ImageProvider}
    ,model{}
{
    connect_socket=std::make_shared<Client>(m_service,"10.252.14.174",2001,this);
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
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receiveLoginInfo(QJsonObject obj)
{
    obj.remove("function");
    if(!obj.isEmpty()){
        netizenId=obj["netizenId"].toString();
        //还有读头像和关注粉丝数量，有了推荐算法后这里还可以读喜好
        //应该把这些基本信息写入客户端本地的配置文件
        //qDebug()<<obj;
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

    data.append('\r');
    connect_socket->do_write(data);

    qDebug()<<"获取更多笔记";
}
void Control::receiveNotes(QJsonObject obj)
{
    //循环遍历每一个笔记
    //把他转换Model的list来源
    obj.remove("function");
    //qDebug()<<obj;
    QStringList keys=obj.keys();
    for(const auto &key:keys){
        QList<QVariant> temp;//noteId
        temp.append(key);
        auto value=obj.value(key).toObject();

        temp.append(value.value("time").toVariant());
        temp.append(value.value("title").toVariant());
        //添加第一张素材的id到model中，内容到imageprovider中
        auto firstImg=value.value("FirstImg").toObject();
        QStringList k=firstImg.keys();
        if(!k.isEmpty()){
            QPixmap image;
            QByteArray data=QByteArray::fromBase64(firstImg.value(k[0]).toString().toUtf8());
            image.loadFromData(data);
            m_material_provider->setPixmap(k[0],image);
            temp.append(k[0]);
        }else{
            temp.append("");
        }
        //添加头像id到model中，内容到imageprovider中
        auto blogger=value.value("Blogger").toObject();
        QPixmap i;
        QByteArray d=QByteArray::fromBase64(blogger.value("profile").toString().toUtf8());
        i.loadFromData(d);
        m_profile_provider->setPixmap(blogger.value("netizenId").toString(),i);
        temp.append(blogger.value("nickname").toVariant());
        temp.append(blogger.value("netizenId").toVariant());

        model.append(temp);
    }
    emit modelChanged();
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
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receiveNoteDetail(QJsonObject obj)
{
    //获取笔记中除了第一张素材以外的素材
    QList<QList<QVariant>> sl;
    //添加内容
    QList<QVariant>content={obj["content"].toVariant()};
    sl.append(content);
    //添加素材（传递的是素材id,内容写到imageprovider）
    auto materials=obj["materials"].toObject();
    QStringList keys=materials.keys();
    QList<QVariant> temp;
    for(const auto &key:keys){
        temp.append(key);
        QPixmap image;
        QByteArray data=QByteArray::fromBase64(materials.value(key).toString().toUtf8());
        image.loadFromData(data);
        m_material_provider->setPixmap(key,image);
    }
    sl.append(temp);
    emit getNoteDetail(sl);
}
void Control::requestPublishNote(QString title,QString content,QList<QString> paths)
{
    QJsonObject noteInfo;
    noteInfo={
        {"function","publish"},
        {"noteId",QUuid::createUuid().toString(QUuid::Id128)},
        {"netizenId",netizenId},
        {"title",title},
        {"content",content}
    };

    QJsonObject materials;
    int order=0;//给素材排序
    for(QString path:paths){
        QJsonObject material;

        path.remove(0,6);//删除file:/

        QImage image(path);
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer,"jpg");
        buffer.close();

        material.insert("order",order++);
        material.insert("image",QString::fromLatin1(imageData.toBase64()));

        QString uuid = QUuid::createUuid().toString(QUuid::Id128);
        materials.insert(uuid,QJsonValue(material));
    }
    noteInfo.insert("materials",QJsonValue(materials));
    noteInfo.insert("materialCount",materials.size());
    QJsonDocument doc(noteInfo);
    QByteArray data=doc.toJson();
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receivePublishNote(QJsonObject obj)
{
    if(obj["result"].toString()=="true"){
        emit getPublishNoteResult(true);
    }else{
        emit getPublishNoteResult(false);
    }
}

void Control::requestPublishComment(QString content,QString noteId)
{
    QJsonObject commentInfo;
    QString commentId=QUuid::createUuid().toString(QUuid::Id128);
    commentInfo={
        {"function","publish_comment"},
        {"commentId",commentId},
        {"netizenId",netizenId},
        {"content",content},
        {"noteId",noteId},
        {"parentId",commentId},
        {"topId",commentId}
    };
    QJsonDocument doc(commentInfo);
    QByteArray data=doc.toJson();
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receivePublishComment(QJsonObject obj)
{
    if(obj["result"].toString()=="true"){
        emit getPublishCommentResult(true);
    }else{
        emit getPublishCommentResult(false);
    }
}

void Control::requestCommentDetail(QString noteId,int count)
{
    QJsonObject commentInfo;
    commentInfo={
        {"function","check_comment"},
        {"noteId",noteId},
        {"count",count}//传递一个已有的评论数量，下次获取的时候在后边接着取
    };
    QJsonDocument doc(commentInfo);
    QByteArray data=doc.toJson();
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receiveCommentDetail(QJsonObject obj)
{
    //遍历每一条评论
    obj.remove("function");
    QList<QList<QVariant>> sl;
    //每个key是评论的id
    QStringList keys=obj.keys();
    for(const auto &key:keys){
        QList<QVariant> temp;
        //添加评论的id
//        temp.append(key);
//        //添加头像
//        QPixmap image;
//        QByteArray data=QByteArray::fromBase64(materials.value(key).toString().toUtf8());
//        image.loadFromData(data);
//        m_provider->setPixmap(key,image);
    }
//    sl.append(temp);
//    emit getNoteDetail(sl);
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
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receiveReplyDetail(QJsonObject obj)
{

}
