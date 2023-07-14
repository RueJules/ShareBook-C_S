#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "control.h"
boost::asio::io_service Client::service;
Client::Client(boost::asio::io_service &io_service, const std::string &host, const int &port, Control *control_ptr)
    :socket(io_service),control{control_ptr},work{new boost::asio::io_service::work(service)}
    ,thread(boost::bind(&boost::asio::io_service::run, &service)) {

    // 异步解析主机和服务
    ip::tcp::endpoint ep(ip::address::from_string(host), port);
    // 异步连接
    socket.async_connect(ep,[this](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "连接成功！" << std::endl;
        }
        else {
            std::cerr << "连接失败：" << ec.message() << std::endl;
        }
    });
}


void Client::do_write(QByteArray data) {

//    if(data==default_data)
//    {
//        //sleep(3);
//        do_read();
//        return;
//    }
    std::lock_guard<std::mutex> lock(write_mutex);
    qDebug()<<data;
    //read_buffer.consume(read_buffer.size());
    auto self(shared_from_this());
    write_buffer = boost::asio::buffer(data.data(),data.size());
    socket.async_write_some(write_buffer,
                             [this,self,data](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                    std::cout << "消息发送成功！" << std::endl;

                                    do_read();
                                 }
                                 else {
                                    std::cerr << "消息发送失败：" << ec.message() << std::endl;
                                 }
                             });
}

void Client::do_read() {

    auto self(shared_from_this());
    boost::asio::async_read_until(socket, read_buffer, "\r", [this, self](const boost::system::error_code &ec, std::size_t length){
        if(!ec){
            qDebug() << "收到回复：";
            std::istream is(&read_buffer);
            QByteArray bytes;
            bytes.resize(length);
            is.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));

            //qDebug()<<bytes;

            QJsonDocument doc(QJsonDocument::fromJson(bytes));
            QJsonObject obj=doc.object();

            //qDebug()<<obj;

            QString function=obj["function"].toString();
            qDebug()<<Qt::endl<<"**********************"<<"::"<<function<<"::"<<Qt::endl;
            if(function=="login"){
                control->receiveLoginInfo(bytes);
            }
            if(function=="view"){
                control->receiveNotes(bytes);

            }
            if(function=="check"){
                control->receiveNoteDetail(bytes);
            }
//            qDebug()<<read_buffer.size();
//            qDebug()<<length;
            if(read_buffer.size()>0)
            {
                do_read();
            }
        }
        else {
            std::cerr << "读取失败：" << ec.message() << std::endl;
        }
    });
}




