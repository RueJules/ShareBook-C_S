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
        //从数据库中找
        std::string cmd="select * from comment where id=\""+commentId.toStdString()+"\"";
        sql::ResultSet *set =query(cmd);
        QString noteId=QString::fromStdString(set->getString(2).c_str());
        QString ownerId=QString::fromStdString(set->getString(3).c_str());
        QString content=QString::fromStdString(set->getString(4).c_str());
        QDateTime time=QDateTime::fromString(QString::fromStdString(set->getString(5).c_str()), "yyyy-MM-dd hh:mm:ss");
        QString parentId=QString::fromStdString(set->getString(6).c_str());
        QString topId=QString::fromStdString(set->getString(7).c_str());
        Comment comment(commentId, ownerId, time,content,noteId,parentId,topId);
        //加到缓存
        old_clean_cache.addToCache(commentId,std::move(comment));
        return &old_clean_cache.getFromCache(commentId);
    }
    return nullptr;
}

//void CommentBroker::createComment(QJsonObject commentObject)
//{
//    if(commentObject.isEmpty()){
//        QString commentId=commentObject["commentId"].toString(); //这条评论（回复）的id
//        QString noteId=commentObject["noteId"].toString(); //评论所属笔记的id（如果是回复就为空）
//        QString ownerId=commentObject["ownerId"].toString();//评论发布者的id
//        QString content=commentObject["content"].toString();//评论的内容
//        QString parentId=commentObject["parentId"].toString(); //评论的是谁的id
//        QString topId=commentObject["topId"].toString(); //这条评论（回复）的最顶级评论的id
//        QDateTime time=QDateTime::currentDateTime();
//        //在服务端新建netizen实例
//        Comment comment(commentId, ownerId, time,content,noteId,parentId,topId);
//        //加到缓存
//        new_cache.addToCache(commentId,std::move(comment));
//        if(commentObject["function"].toString()=="insert"){
//            new_cache.addToCache(commentId,std::move(comment));
//            return;
//        }

//        if(commentObject["function"].toString()=="update"){
//            if(new_cache.inCache(commentId)){
//                new_cache.deleteFromCache(commentId);
//                new_cache.addToCache(commentId,std::move(comment));
//                return;
//            }

//            if(old_dirty_cache.inCache(commentId)){
//                old_dirty_cache.deleteFromCache(commentId);
//            }

//            old_dirty_cache.addToCache(commentId,std::move(comment));
//            return;
//        }

//        if(commentObject["function"].toString()=="delete"){
//            if(new_cache.inCache(commentId)){
//                new_cache.deleteFromCache(commentId);
//                return;
//            }
//            old_delete_cache.addToCache(commentId,std::move(comment));
//        }

//    }
//}


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

//void CommentBroker::old_clean_cache_sycn()
//{
//        //

//        //        const std::unordered_map<QString,Comment>& caches = m_cache.getCacheConst();
//        const std::unordered_map<QString,Comment>& caches = new_cache.getCacheConst();
//        for(auto cache :caches ){
//        QString id;
//        QString owner_id;
//        QString time;
//        QString content;
//        QString note_id;
//        QString parent_id;
//        QString top_id;
//        cache.second.get_info(id,owner_id,time,content,note_id,parent_id,top_id);
//        QString cmd=QString("")+"INSERT INTO comment  VALUES "+"("+id+","+ note_id+","+ owner_id+","+ content+","+time+","+ parent_id+","+ top_id+")";
//        sql::ResultSet *set =query(cmd.toStdString());
//        }
//}

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
//void CommentBroker::start_thread()
//{
//    m_sycn_thread=new std::thread(&CommentBroker::sycn,this);

//}
