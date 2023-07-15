#include "comment.h"

Comment::Comment(QString id, QString owner_id, QDateTime time, QString content, QString note_id, QString parent_id, QString top_id):CommentInterface{id},m_ownerId{owner_id},m_time{time},m_content{content},m_noteId{note_id},m_parentId{parent_id},m_topId{top_id}
{

}

QJsonObject Comment::getDetails()
{

}

void Comment::get_info(QString &id, QString &owner_id, QString &time, QString &content, QString &note_id, QString &parent_id, QString &top_id)
{
    id=get_id();
    owner_id=m_ownerId;
    time=m_time.toString();
    content=m_content;
    note_id=m_noteId;
    parent_id=m_parentId;
    top_id=m_topId;
}
