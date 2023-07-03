#ifndef COMMENTINTERFACE_H
#define COMMENTINTERFACE_H

#include <QString>
#include <QJsonObject>
class CommentInterface
{

public:
    CommentInterface(QString new_id);
    QString get_id();
    virtual QJsonObject getDetails()=0;

private:
    QString m_id; //评论id
};

#endif // COMMENTINTERFACE_H
