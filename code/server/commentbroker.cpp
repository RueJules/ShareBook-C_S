#include "commentbroker.h"
#include <mutex>
#include <unordered_map>
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
        std::lock_guard<std::mutex> lock(commentBrokerMutex);
        if (s_commentBroker == nullptr) {
            auto temp = std::shared_ptr<CommentBroker>(new CommentBroker());
            s_commentBroker = temp;
        }
    }
    return s_commentBroker;
}

Comment *CommentBroker::findById(QString commentId)
{
    if(old_clean_cache.inCache(commentId)){
        return &old_clean_cache.getFromCache(commentId);
    }
    else{
        if(new_cache.inCache(commentId))
        {
            return &new_cache.getFromCache(commentId);
        }else{
            if(old_dirty_cache.inCache(commentId))
            {
                return &old_dirty_cache.getFromCache(commentId);

            }else{
                qDebug() << "commenBroker nonono\n";
            }
        }

    }
    return nullptr;
}

bool CommentBroker::createComment(QJsonObject commentObject){

    if(!commentObject.isEmpty()){
        QString commentId=commentObject["commentId"].toString(); //这条评论（回复）的id
        QString noteId=commentObject["noteId"].toString(); //评论所属笔记的id（如果是回复就为空）
        QString ownerId=commentObject["netizenId"].toString();//评论发布者的id
        QString content=commentObject["content"].toString();//评论的内容
        QString parentId=commentObject["parentId"].toString(); //评论的是谁的id
        QString topId=commentObject["topId"].toString(); //这条评论（回复）的最顶级评论的id
        QDateTime time=QDateTime::currentDateTime();
        //在服务端新建netizen实例
        Comment comment(commentId, ownerId, time,content,noteId,parentId,topId);
        //加到缓存
        new_cache.addToCache(commentId,std::move(comment));
        return new_cache.inCache(commentId) ? true : false;
    }
    return false;
}

void CommentBroker::initCache()
{
    //涉及的表有，comment
    std::string cmd="select * from comment";
    sql::ResultSet *set =query(cmd);
    std::lock_guard<std::mutex> lock(commentBrokerMutex);
    while (set->next()) {
        QString id=QString::fromStdString(set->getString("id").c_str());
        QString owner_id=QString::fromStdString(set->getString("owner_id").c_str());
        QDateTime time=QDateTime::fromString(QString::fromStdString(set->getString("time").c_str()), "yyyy-MM-dd hh:mm:ss");
        QString content=QString::fromStdString(set->getString("content").c_str());
        QString note_id=QString::fromStdString(set->getString("note_id").c_str());
        QString parent_id=QString::fromStdString(set->getString("parent_id").c_str());
        QString top_id=QString::fromStdString(set->getString("top_id").c_str());
        Comment temp(id, owner_id, time, content, note_id, parent_id, top_id);
        //        m_cache.addToCache(id,std::move(temp));
        old_clean_cache.addToCache(id,std::move(temp));
    }
    qDebug() << "comment inint finish !\n";

}

//void CommentBroker::sycn()
void CommentBroker::new_cache_sycn()
{
    //    将Cache中的对象同步到数据库中

    //        const std::unordered_map<QString,Comment>& caches = m_cache.getCacheConst();
    const std::unordered_map<QString,Comment>& caches = new_cache.getCacheConst();
    for(auto cache :caches ){
        QString id;
        QString owner_id;
        QString time;
        QString content;
        QString note_id;
        QString parent_id;
        QString top_id;
        cache.second.get_info(id,owner_id,time,content,note_id,parent_id,top_id);
        QString cmd=QString("")+"INSERT INTO comment  VALUES "+"(\""+id+"\",\""+ note_id+"\",\""+ owner_id+"\",\""+ content+"\",\""+time+"\",\""+ parent_id+"\",\""+ top_id+"\")";
        qDebug() << cmd << '\n';
        sql::ResultSet *set =query(cmd.toStdString());
    }
}

void CommentBroker::old_dirty_cache_sycn()
{
    //UPDATE comment SET  note_id=?, owner_id=?, content=?, time=?, parent_id=?, top_id=? where id=?

    //        const std::unordered_map<QString,Comment>& caches = m_cache.getCacheConst();
    const std::unordered_map<QString,Comment>& caches = old_dirty_cache.getCacheConst();
    for(auto cache :caches ){
        QString id;
        QString owner_id;
        QString time;
        QString content;
        QString note_id;
        QString parent_id;
        QString top_id;
        cache.second.get_info(id,owner_id,time,content,note_id,parent_id,top_id);
        QString cmd=QString("")+"UPDATE comment SET "+"note_id=\""+ note_id+"\", owner_id=\""+ owner_id+"\", content=\""+ content+"\", time=\""+time+"\", parent_id=\""+ parent_id+"\", top_id=\""+ top_id+"\" where id=\""+id + "\"";
        qDebug() << cmd << '\n';
        sql::ResultSet *set =query(cmd.toStdString());
    }

}

void CommentBroker::old_delete_cache_sycn()
{
    //DELETE FROM comment

    //        const std::unordered_map<QString,Comment>& caches = m_cache.getCacheConst();
    const std::unordered_map<QString,Comment>& caches = old_delete_cache.getCacheConst();
    for(auto cache :caches ){
        QString id;
        QString owner_id;
        QString time;
        QString content;
        QString note_id;
        QString parent_id;
        QString top_id;
        cache.second.get_info(id,owner_id,time,content,note_id,parent_id,top_id);
        QString cmd=QString("")+"DELETE FROM comment "+"where id=\""+id+"\"";
        qDebug() << cmd << '\n';
        drop(cmd.toStdString());
    }
}

void CommentBroker::sycn()
{
    qDebug()<<"commentBroker的同步--------------------------------------------------------------------";

    new_cache_sycn();
    old_dirty_cache_sycn();
    old_delete_cache_sycn();
    old_delete_cache.clearCache();
    old_clean_cache.clearCache();
    old_dirty_cache.clearCache();
    new_cache.clearCache();
    initCache();

}

