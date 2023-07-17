#ifndef COMMENT_H
#define COMMENT_H

#include "commentinterface.h"
#include <QString>
#include <QDateTime>
class Comment:public CommentInterface
{
public:
    Comment(QString id,QString owner_id,QDateTime time,QString content,QString note_id,QString parent_id,QString top_id);
    QJsonObject getDetails()override;//评论的内容
    void get_info(QString &id, QString &owner_id, QString &time, QString &content, QString &note_id, QString &parent_id, QString &top_id);
private:
    QString m_ownerId;
    QString m_content;
    QString m_noteId;
    QString m_parentId;
    QString m_topId;
    QDateTime m_time;
};

#endif // COMMENT_H
