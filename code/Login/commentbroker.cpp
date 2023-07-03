#include "commentbroker.h"
#include <mutex>
std::shared_ptr<CommentBroker> CommentBroker::s_commentBroker=nullptr;
std::mutex CommentBroker::commentBrokerMutex;

CommentBroker::~CommentBroker()
{

}
CommentBroker::CommentBroker()
{

}
std::shared_ptr<CommentBroker> CommentBroker::getInstance()
{
    //可以用call_once和once_flag替换
    if (s_commentBroker == nullptr) {
        std::unique_lock<std::mutex> lock(commentBrokerMutex);
        if (s_commentBroker == nullptr) {
            auto temp = std::shared_ptr<CommentBroker>(new CommentBroker());
            s_commentBroker = temp;
        }
    }
    return s_commentBroker;
}

Comment *CommentBroker::findById(QString commentId)
{
    if(m_cache.inCache(commentId)){
        return &m_cache.getFromCache(commentId);
    }
    else{
        //从数据库中找
        std::string cmd="select * from comment where id=\""+commentId.toStdString()+"\"";
        sql::ResultSet *set =query(cmd);
        QString noteId=QString::fromStdString(set->getString(2).c_str());
        QString ownerId=QString::fromStdString(set->getString(3).c_str());
        QString content=QString::fromStdString(set->getString(4).c_str());
        QDateTime time=QDateTime::fromString(QString::fromStdString(set->getString(5).c_str()), "yyyyMMddhhmmss");
        QString parentId=QString::fromStdString(set->getString(6).c_str());
        QString topId=QString::fromStdString(set->getString(7).c_str());
        Comment comment(commentId, ownerId, time,content,noteId,parentId,topId);
        //加到缓存
        m_cache.addToCache(commentId,std::move(comment));
        return &m_cache.getFromCache(commentId);
    }
    return nullptr;
}
void CommentBroker::createComment(QJsonObject commentObject){
    if(commentObject.isEmpty()){
        QString commentId=commentObject["commentId"].toString();
        QString noteId=commentObject["noteId"].toString();
        QString ownerId=commentObject["ownerId"].toString();
        QString content=commentObject["content"].toString();
        QString parentId=commentObject["parentId"].toString();
        QString topId=commentObject["topId"].toString();
        QDateTime time=QDateTime::currentDateTime();
        //在服务端新建netizen实例
        Comment comment(commentId, ownerId, time,content,noteId,parentId,topId);
        //加到缓存
        m_cache.addToCache(commentId,std::move(comment));
    }
}

void CommentBroker::initCache()
{
    //涉及的表有，comment
}

void CommentBroker::sycn()
{

}
