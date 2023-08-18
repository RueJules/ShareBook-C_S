#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include"boostnetwork.h"
#include <string>
#include "threadsafe_queue.h"

using namespace boost::asio;



class Server:public std::enable_shared_from_this<Server>
{
public:
    Server();
    void startAccept();
    void startCommunicate();

    static ThreadSafe_queue<std::string> videoData;
    static ThreadSafe_queue<std::string> path;
private:
    tcp::acceptor m_acc;
    static io_service service_2001;
    static io_service service_9999;




    //保证service.run不会结束
    boost::shared_ptr<io_service::work> work2001;
    boost::shared_ptr<io_service::work> work9999;
    boost::thread_group threads;
    //std::vector<std::shared_ptr<BoostNetwork>> sockets;

};

#endif // SERVER_H
