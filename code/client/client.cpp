#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "control.h"
boost::asio::io_service Client::service;
Client::Client(boost::asio::io_service &io_service, const std::string &host, const int &port, Control *control_ptr)
    :socket(io_service),control{control_ptr},work{new boost::asio::io_service::work(service)}
    ,thread(boost::bind(&boost::asio::io_service::run, &service)),default_data() {

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

    if(data==default_data)
    {
        do_read();
    }
    std::lock_guard<std::mutex> lock(write_mutex);
    //qDebug()<<data;
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
                                 //saved_data = data;
                                 //can_write=false;
                             });
}

void Client::do_read() {


    auto self(shared_from_this());
  //  bool can_write = false;
//    std::shared_ptr<deadline_timer> timer=std::make_shared<deadline_timer>(service,boost::posix_time::seconds(20));
//    timer->async_wait([this,self,timer](boost::system::error_code ec){
//            if(!ec){
//                do_write(saved_data);
//            }
//        }
//    );

    boost::asio::async_read_until(socket, read_buffer, "}\n\r", [this, self](const boost::system::error_code &ec, std::size_t length){
        if(!ec){
            std::string data(boost::asio::buffers_begin(read_buffer.data()), boost::asio::buffers_end(read_buffer.data()));
            QByteArray recArray(data.c_str());
            qDebug() << "收到回复：";
//            const char* data = boost::asio::buffer_cast<const char*>(read_buffer.data());
//            QByteArray bytes=QByteArray::fromRawData(data, length);
            qDebug()<<recArray;
            QJsonDocument doc(QJsonDocument::fromJson(recArray));
            QJsonObject obj=doc.object();
            QString function=obj["function"].toString();
            if(function=="login"){
                control->receiveLoginInfo(recArray);

            }
            if(function=="view"){
                control->receiveNotes(recArray);

            }
            read_buffer.consume(length);
            if(length!=0)
            {
                std::cout<<Qt::endl<<"**********************"<<"::"<<length<<"::"<<Qt::endl;
                do_write(default_data);
            }

        }
        else {

            std::cerr << "读取失败：" << ec.message() << std::endl;
            qDebug() << default_data;
            do_write(default_data);
        }
    });
}




