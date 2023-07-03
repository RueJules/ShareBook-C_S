#include "commentproxy.h"

CommentProxy::CommentProxy(QString id):CommentInterface{id},m_comment{nullptr}
{

}
CommentProxy::CommentProxy(QString id, std::unique_ptr<Comment>&&comment):CommentInterface{id},m_comment{std::move(comment)}
{

}

QJsonObject CommentProxy::getDetails()
{

}
