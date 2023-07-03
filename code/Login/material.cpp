/*Auther:Huangyihong
Date:2023.6.17*/

#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QPixmap>
#include <QBuffer>

#include "material.h"

Material::Material(QString id, QString imgsrc, QString note_id, int order):MaterialInterface(id),m_imgsrc{imgsrc},m_noteId{note_id},m_order{order}
{

}

QJsonObject Material::getDetails()
{
    //根据图片路径在本地图库中找到图片并转码
    QPixmap image;
    image.load("/root/sharebook/materials" + m_imgsrc);
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer,"jpg", 1);
    buffer.close();

    QJsonObject imgJson{

        {"ImgData",QJsonValue::fromVariant(imageData.toBase64())},

    };
    return imgJson;
}
