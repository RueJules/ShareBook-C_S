/*Auther:Huangyihong
Date:2023.6.17*/

#ifndef MATERIAL_H
#define MATERIAL_H
#include <QByteArray>
#include <QPixmap>
#include "materialInterface.h"

class Material : public MaterialInterface {

public:
    Material(QString id,QString imgsrc,QString note_id,int order);
    QJsonObject getDetails() override;
    QJsonObject getVideo() override;
    void getInfo(QString &id,QString &imgsrc,QString &note_id,int &order);


private:
    QString m_imgsrc; //当前素材的内容（路径）
    QString m_noteId;//当前素材所属的笔记
    int m_order; //图片所属顺序

};

#endif
