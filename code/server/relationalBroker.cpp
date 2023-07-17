#include "relationalBroker.h"
#include "netizenBroker.h"
#include "noteBroker.h"
#include "commentbroker.h"
#include "materialBroker.h"
#include <iostream>
#include <QDebug>
#include<string>

std::unique_ptr<sql::Connection> RelationalBroker::m_conn=NULL;

RelationalBroker::RelationalBroker()
{
//    //安装驱动
//    sql::Driver* driver = sql::mariadb::get_driver_instance();

//    //配置连接
//    sql::SQLString url("jdbc:mariadb://10.252.49.215:3306/ShareBook");
//    sql::Properties properties({{"user", "ShareBook"}, {"password", "12345678"}});

//    //建立连接
//    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
//    m_conn=std::move(conn);

    //qDebug() << "over\n";
}

void RelationalBroker::initDataBase()
{
    //安装驱动
    sql::Driver* driver = sql::mariadb::get_driver_instance();

    //配置连接
    sql::SQLString url("jdbc:mariadb://10.252.148.137:3306/ShareBook");
    sql::Properties properties({{"user", "ShareBook"}, {"password", "12345678"}});

    //建立连接
    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
    m_conn=std::move(conn);

    NetizenBroker::getInstance()->initCache();
    NoteBroker::getInstance()->initCache();
    CommentBroker::getInstance()->initCache();
    MaterialBroker::getInstance()->initCache();
    //qDebug() << "over\n";

}

sql::ResultSet* RelationalBroker::query(std::string cmd)
{
    if(m_conn == NULL){
        qDebug() << "NOOOOOOOOO\n";
    }
    try{
        std::unique_ptr<sql::PreparedStatement> stmntQ(m_conn->prepareStatement(cmd));
        sql::ResultSet *res = stmntQ->executeQuery();
        return res;

    }catch(sql::SQLException &e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    return nullptr;
}

void RelationalBroker::insert(std::string cmd)
{
    try{

    std::unique_ptr<sql::PreparedStatement> stmnt(m_conn->prepareStatement(cmd));
    stmnt->executeUpdate();

    }catch(sql::SQLException& e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

void RelationalBroker::update(std::string cmd)
{
    try{

        std::unique_ptr<sql::PreparedStatement> stmnt(m_conn->prepareStatement(cmd));
        stmnt->executeUpdate();

    }catch(sql::SQLException& e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

void RelationalBroker::drop(std::string cmd)
{
    try{

        std::unique_ptr<sql::PreparedStatement> stmnt(m_conn->prepareStatement(cmd));
        stmnt->executeUpdate();

    }catch(sql::SQLException& e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

void RelationalBroker::sycn()
{
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(FRESH_TIME));
        NetizenBroker::getInstance()->sycn_signIn();
        NoteBroker::getInstance()->sycn_new();
        NetizenBroker::getInstance()->sycn();
        CommentBroker::getInstance()->sycn();
        NoteBroker::getInstance()->sycn();
        MaterialBroker::getInstance()->sycn();
    }
}
