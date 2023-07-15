#ifndef COMMENTBROKER_H
#define COMMENTBROKER_H

#include "relationalBroker.h"
#include "comment.h"
#include <thread>
class CommentBroker:public RelationalBroker
{
public:
    ~CommentBroker();
    static  std::shared_ptr<CommentBroker> getInstance();
    Comment *findById(QString commentId);
    bool createComment(QJsonObject commentObject);//添加评论到相应的cache
    //    void createComment(QJsonObject commentObject);
    void initCache();//同步数据库，把数据库的内容写到old_clean_cache里
    void new_cache_sycn();//同步数据库，把new_cache的内容写回数据库
    void old_clean_cache_sycn();
    void old_dirty_cache_sycn();
    void old_delete_cache_sycn();
    void sycn();
    void start_thread();

private:
    CommentBroker();//私有构造函数，单例模式

    static std::shared_ptr<CommentBroker> s_commentBroker;  //代管者实例
    static std::mutex commentBrokerMutex;
    std::thread * m_sycn_thread;

    //    Cache<Comment>m_cache;
    Cache<Comment> new_cache;//insert to
    Cache<Comment> old_clean_cache;//select
    Cache<Comment> old_dirty_cache;//updata
    Cache<Comment> old_delete_cache;//delete

};

#endif // COMMENTBROKER_H
