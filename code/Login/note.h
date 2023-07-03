/*Auther:GongYanxian
Date:2023.6.19*/
#ifndef NOTE_H
#define NOTE_H
#include"noteInterface.h"
#include<QDateTime>
#include<string>
#include <map>
#include"materialProxy.h"
class Note :public NoteInterface {

public:
    Note(QString id, QString title, QString content,QDateTime time,int materialCount,QString blogger);//从数据库读取数据生成笔记
   /* Note(QString id, QString title, QString content,QDateTime time,int materialCount,QString blogger,std::map<MaterialProxy> &&materialList);*///从数据库读取数据生成笔记
//    void addMaterial(QString materialId,MaterialProxy&& material)override;
    void addMaterial(int order,MaterialProxy&& material)override;
    QJsonObject getNoteAbstract() override;

private:
    QString m_bloggerId; //博主id
    QString  m_title;
    QString  m_content;
    QDateTime m_time;
    int m_materials;
    //std::map<QString, MaterialProxy> m_materialList;//初始化为空，点击查看详情时再加入列表中
    std::map<int, MaterialProxy> m_materialList;
};

#endif
