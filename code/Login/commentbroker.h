#ifndef COMMENTBROKER_H
#define COMMENTBROKER_H

#include "relationalBroker.h"
#include "comment.h"
class CommentBroker:public RelationalBroker
{
public:
    ~CommentBroker();
    static  std::shared_ptr<CommentBroker> getInstance();
    Comment *findById(QString commentId);
    void createComment(QJsonObject commentObject);
    void initCache();//同步数据库，把数据库的内容写到cache里
    void sycn();//同步数据库，把cache的内容写回数据库

private:
    CommentBroker();//私有构造函数，单例模式

    static std::shared_ptr<CommentBroker> s_commentBroker;  //代管者实例
    static std::mutex commentBrokerMutex;

    Cache<Comment>m_cache;
};

#endif // COMMENTBROKER_H
