#include "socket.h"
#include "server.h"
#include <string>
#include <iostream>
#include <QDebug>

Socket::Socket(boost::asio::io_service &service, const std::string &host, const int &port):m_socket(service)
{
    tcp::endpoint ep(ip::address::from_string(host), port);
    m_socket.async_connect(ep,[this](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "连接成功！" << std::endl;
        }
        else {
            std::cerr << "连接失败：" << ec.message() << std::endl;
        }

    });
}

void Socket::do_write()
{
    //material ID,video data
    std::string tmp;
    Server::videoData.wait_and_pop(tmp);
    QJsonObject video;
    std::string uuid= tmp.substr(0, 32);
    std::string p = tmp.substr(32,3);
    std::string videoData = tmp.substr(35);
    video = {

            {"data", QString::fromStdString(videoData)},
            {"postfix", QString::fromStdString(p)},
            {"id", QString::fromStdString(uuid)},

    };

//    video = {

//        {"postfix", "QString::fromStdString(p)"},
//        {"id", "QString::fromStdString(id)"},
//        {"data", "QString::fromStdString(tmp.second)"}
//    };
    qDebug() << "id:" << video["id"] << "p:" << video["postfix"] << '\n';
    QJsonDocument videoDoc;
    videoDoc.setObject(video);
    QByteArray bytes = videoDoc.toJson();
    bytes.append('\r');
    qDebug() << bytes.size() << "ready!! \n";
    auto self(shared_from_this());
    write_buffer = boost::asio::buffer(bytes.data(),bytes.size());
    async_write(m_socket, write_buffer,
                            [this,self,bytes](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                        qDebug() << "消息发送成功！\n";
                                        qDebug() << length <<'\n';
                                        do_read();
                                }
                                else {
                                    std::cerr << "消息发送失败：" << ec.message() << std::endl;
                                }
                            });

}

void Socket::do_read()
{

    auto self(shared_from_this());
    m_socket.async_read_some(boost::asio::buffer(read_buffer,1024), [this, self](const boost::system::error_code &e, std::size_t length){
        if(!e){
            qDebug() <<  "vhherfvh:" << length;
            if(length > 0)
            {
                std::string str{read_buffer};
                std::cout << str<<Qt::endl;
                //提取出来视频的id
                qDebug() <<  "From Server :"<< QString::fromStdString(str);
                Server::path.push(str);
            }

            do_write();

        }else{
            std::cout << "没东西\n";
        }

    });

}

void Socket::start()
{
    do_write();
}


