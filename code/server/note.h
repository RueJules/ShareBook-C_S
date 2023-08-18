/*Auther:GongYanxian
Date:2023.6.19*/
#ifndef NOTE_H
#define NOTE_H
#include "noteInterface.h"
#include <QDateTime>
#include <string>
#include <map>
#include "materialProxy.h"
#include <QList>

class Note :public NoteInterface {

public:
    Note(QString id, QString title, QString content,QDateTime time,int materialCount,QString blogger, QList<QString> &&comments);//从数据库读取数据生成笔记
    Note(QString id, QString title, QString content,QDateTime time,int materialCount,QString blogger);

    void addMaterial(int order,MaterialProxy &&material)override;
    QJsonObject getNoteAbstract() override;
    QJsonObject getNoteDetails() override;
    QJsonObject toDB();
    void addComment(QString commentId) override;
    void commentList(int flag,QList<QString> &commentsId);
    bool isVideo() override;

private:
    QString m_bloggerId; //博主id
    QString  m_title;
    QString  m_content;
    QDateTime m_time;
    int m_materials;

    QList<QString> m_commentList;//维护一个这个笔记存在的评论ID列表，建立了笔记和笔记评论之间的一种关系，每次返回某条笔记的一些评论时就从此处获取存在的评论id,每次从什么地方开始取由客户端那边已经有的评论数量传过来作为读取的标记位
    std::map<int, MaterialProxy> m_materialList;
};

#endif
