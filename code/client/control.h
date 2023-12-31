#ifndef CONTROL_H
#define CONTROL_H

#include "imageprovider.h"
#include"client.h"
#include <memory>
#include <boost/thread.hpp>
#include <QObject>
#include <QVariant>
#include <QJsonObject>

class Control :public QObject{
    Q_OBJECT
public:
    Q_PROPERTY(QList<QList<QVariant>> model READ getModel WRITE setModel NOTIFY modelChanged)
    Control();
    ImageProvider* getMaterialProvider() { return m_material_provider; }
    ImageProvider* getProfileProvider() { return m_profile_provider; }
    QList<QList<QVariant>> getModel(){return model;}
    void setModel(QList<QList<QVariant>> m){if(m!=model)model=m;}


    Q_INVOKABLE void requestLogin(QString nickname, QString psw);
    Q_INVOKABLE void receiveLoginInfo(QJsonObject obj);
    Q_INVOKABLE void requestNotes();
    Q_INVOKABLE void receiveNotes(QJsonObject obj);
    Q_INVOKABLE void requestNoteDetail(QString noteId);
    Q_INVOKABLE void receiveNoteDetail(QJsonObject obj);
    Q_INVOKABLE void requestPublishNote(QString title,QString content,QList<QString> paths);
    Q_INVOKABLE void receivePublishNote(QJsonObject obj);
    Q_INVOKABLE void requestPublishComment(QString content,QString noteId);
    Q_INVOKABLE void receivePublishComment(QJsonObject obj);
    Q_INVOKABLE void requestCommentDetail(QString noteId,int count);
    Q_INVOKABLE void receiveCommentDetail(QJsonObject obj);
    Q_INVOKABLE void requestReplyDetail(QString commentId);
    Q_INVOKABLE void receiveReplyDetail(QJsonObject obj);

signals:
    void getAccountInfo(bool res);
    void getPublishNoteResult(bool res);
    void getPublishCommentResult(bool res);
    void getPublishReplyResult(bool res);
    void getNoteDetail(QList<QList<QVariant>> NoteDetail);
    void getCommentDetail(QList<QList<QVariant>> commentDetail);
    void getReplyDetail(QList<QList<QVariant>> commentDetail);
    void modelChanged();
private:
    QList<QList<QVariant>> model;//传给qml的浏览笔记的缩略图model
    ImageProvider *m_material_provider;
    ImageProvider *m_profile_provider;

    static boost::asio::io_service m_service;
    boost::shared_ptr<io_service::work> work;    //保证service.run不会结束
    std::shared_ptr<Client> connect_socket;
    boost::thread_group threads;

    QString netizenId;

};


#endif // CONTROL_H
