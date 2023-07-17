#include "commentproxy.h"
#include "commentbroker.h"

CommentProxy::CommentProxy(QString id):CommentInterface{id},m_comment{nullptr}
{

}
CommentProxy::CommentProxy(QString id, std::unique_ptr<Comment>&&comment):CommentInterface{id},m_comment{std::move(comment)}
{

}

QJsonObject CommentProxy::getDetails()
{
    if(m_comment == nullptr)
    {
        Comment *comment = CommentBroker::getInstance()->findById(get_id());
        m_comment = std::make_unique<Comment>(*comment);
    }
    m_comment->getDetails()
}
