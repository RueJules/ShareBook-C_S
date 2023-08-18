/*Auther:Huangyihong
Date:2023.6.17*/

#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QPixmap>
#include <QBuffer>
#include "server.h"

#include "material.h"

Material::Material(QString id, QString imgsrc, QString note_id, int order):MaterialInterface(id),m_imgsrc{imgsrc},m_noteId{note_id},m_order{order}
{

}

QJsonObject Material::getDetails()
{
    //根据图片路径在本地图库中找到图片并转码
    QPixmap image;
    qDebug() << m_imgsrc;
    image.load(m_imgsrc);
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer,"jpg", 50);
    buffer.close();
    qDebug ()<< "material id------" << get_id()<<'\n';

    QJsonObject imgJson{

        {get_id(),QJsonValue::fromVariant(imageData.toBase64())},

    };
    return imgJson;
}

QJsonObject Material::getVideo()
{
    QJsonObject videoJson{
        {get_id(), m_imgsrc}
    };
    return videoJson;
}

void Material::getInfo(QString &id, QString &imgsrc, QString &note_id, int &order)
{
    id=get_id();

    if(m_imgsrc==""){
        std::string strPath;
        //Server::path.wait_and_pop(id.toStdString(), strPath);
        m_imgsrc = QString::fromStdString(strPath);
    }
    imgsrc=m_imgsrc;
    note_id=m_noteId;
    order=m_order;
}
