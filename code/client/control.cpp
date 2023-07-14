#include "control.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QList>
#include <QPixmap>
#include <QBuffer>
#define THREAD_COUNT 3

boost::asio::io_service Control::m_service;

Control::Control():work{new boost::asio::io_service::work(m_service)},m_provider{new ImageProvider},model{}
{
    connect_socket=std::make_shared<Client>(m_service,"10.252.77.255",2001,this);
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

void Control::receiveLoginInfo(QByteArray data)
{
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
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
void Control::receiveNotes(QByteArray data)
{
    //循环遍历每一个笔记
    //把他转换Model的list来源
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
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
        QPixmap image;
        QByteArray data=QByteArray::fromBase64(firstImg.value(k[0]).toString().toUtf8());
        image.loadFromData(data);
        m_provider->setPixmap(k[0],image);
        temp.append(k[0]);
        //添加头像id到model中，内容到imageprovider中
        auto blogger=value.value("Blogger").toObject();
        QPixmap i;
        QByteArray d=QByteArray::fromBase64(blogger.value("profile").toString().toUtf8());
        i.loadFromData(d);
        m_provider->setPixmap(blogger.value("netizenId").toString(),i);

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

void Control::receiveNoteDetail(QByteArray data)
{
    //获取笔记中除了第一张素材以外的素材
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
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
        m_provider->setPixmap(key,image);
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

    QJsonDocument doc(noteInfo);
    QByteArray data=doc.toJson();
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receivePublishNote(QByteArray data)
{
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj=doc.object();
    if(obj["result"].toString()=="true"){
        qDebug("发送结果1");
        emit getPublishResult(true);
    }else{
        qDebug("发送结果2");
        emit getPublishResult(false);
    }
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
    data.append('\r');
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
    data.append('\r');
    connect_socket->do_write(data);
}

void Control::receiveReplyDetail(QByteArray data)
{

}
