/*Auther:Huangyihong
Date:2023.6.17*/

#include <QJsonObject>
#include <QPixmap>
#include <QBuffer>

#include "material.h"

Material::Material(QString id, QString imgsrc, QString note_id, int order):MaterialInterface(id),m_imgsrc{imgsrc},m_noteId{note_id},m_order{order}
{

}

QJsonObject Material::getDetails()
{
    QPixmap image;
    image.load("/root/resource/Screenshot_20230629_200927.png");
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer,"jpg", 1);
    buffer.close();

    QJsonObject imgJson{
        {"source","ffgf"},
        {"order", m_order}
    };
    return imgJson;
}
