#ifndef COMMENTPROXY_H
#define COMMENTPROXY_H
#include "comment.h"
#include "commentinterface.h"
#include <memory>
#include <QString>
class CommentProxy:public CommentInterface
{
public:
    CommentProxy(QString id);
    CommentProxy(QString id, std::unique_ptr<Comment> &&comment);
    QJsonObject getDetails()override;

private:
    std::unique_ptr<Comment> m_comment; //持有一个素材实例
};

#endif // COMMENTPROXY_H
