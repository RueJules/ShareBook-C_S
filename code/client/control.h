#ifndef CONTROL_H
#define CONTROL_H

#include "imageprovider.h"
#include"client.h"
#include <memory>
#include <boost/thread.hpp>
#include <QObject>
#include <QVariant>

class Control :public QObject{
    Q_OBJECT
public:
    Control();
    ImageProvider* getImageProvider() { return m_provider; }

    Q_INVOKABLE void requestLogin(QString nickname, QString psw);
    Q_INVOKABLE void receiveLoginInfo(QByteArray data);
    Q_INVOKABLE void requestNotes();
    Q_INVOKABLE void receiveNotes(QByteArray data);
    Q_INVOKABLE void requestNoteDetail(QString noteId);
    Q_INVOKABLE void receiveNoteDetail(QByteArray data);
    Q_INVOKABLE void requestCommentDetail(QString noteId);
    Q_INVOKABLE void receiveCommentDetail(QByteArray data);
    Q_INVOKABLE void requestReplyDetail(QString commentId);
    Q_INVOKABLE void receiveReplyDetail(QByteArray data);

signals:
    void getNewNotes(QList<QList<QVariant>> newNotes);
    void getAccountInfo(bool res);
    void getNoteDetail(QList<QList<QVariant>> NoteDetail);
    void getCommentDetail(QList<QList<QVariant>> NoteDetail);
    void getReplyDetail(QList<QList<QVariant>> NoteDetail);
private:
    static boost::asio::io_service m_service;
    ImageProvider *m_provider;
    //保证service.run不会结束
    boost::shared_ptr<io_service::work> work;
    std::shared_ptr<Client> connect_socket;
    boost::thread_group threads;

    QString netizenId;

};


#endif // CONTROL_H
