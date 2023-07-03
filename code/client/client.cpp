#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include "control.h"
Client::Client(boost::asio::io_service &io_service, const std::string &host, const int &port, Control *control_ptr)
    :socket(io_service),control{control_ptr} {

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

    std::lock_guard<std::mutex> lock(write_mutex);
    qDebug()<<data;
    auto self(shared_from_this());
    data.append('\r');
    write_buffer = boost::asio::buffer(data.data(),data.size());
    socket.async_write_some(write_buffer,
                             [this,self](boost::system::error_code ec, std::size_t /*length*/) {
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
    boost::asio::async_read_until(socket, read_buffer, '\r',
    [this,self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            qDebug() << "收到回复：";
            read_buffer.commit(length);
            const char* data = boost::asio::buffer_cast<const char*>(read_buffer.data());
            QByteArray bytes=QByteArray::fromRawData(data, length);
            QJsonDocument doc(QJsonDocument::fromJson(bytes));
            QJsonObject obj=doc.object();
            QString function=obj["function"].toString();
            if(function=="login"){
                control->receiveLoginInfo(QByteArray::fromRawData(data, length));

            }
            if(function=="view"){
                control->receiveNotes(QByteArray::fromRawData(data, length));

            }
            read_buffer.consume(length);
        }
        else {
            std::cerr << "读取失败：" << ec.message() << std::endl;
        }
    });
}
