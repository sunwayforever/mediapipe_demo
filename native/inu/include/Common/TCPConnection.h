
#ifndef _TCPCONNECTION_H__
#define _TCPCONNECTION_H__

#include "CommonUtilitiesDefs.h"

#ifdef _MSC_VER
// define windows XP
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif
// This definition comes to eleminate many warnings from boost of
// [-Wunused-variable]
#define BOOST_SYSTEM_NO_DEPRECATED 1

#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <string>

#define INU_SERVER_PORT "52321"
#define INU_CLIENT_PORT "52322"

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace InuDev {
//
//
//	CTcpClientConnector
//
/////////////////////////////////////////////////////////

class COMMONUTILITIES_API CTcpClientConnector {
   public:
    CTcpClientConnector(std::string ip, std::string port);

    ~CTcpClientConnector();

    void Write(const std::string& ip,
               std::vector<unsigned char>& writeBuffer) const;
    void Write(unsigned long ipV4,
               std::vector<unsigned char>& writeBuffer) const;

    int Read(std::vector<unsigned char>& readResult, unsigned long& senderIP);
    void Connect(const std::string& ip) const;
    void Close();

   private:
    std::string mIp;
    std::string mPort;
    mutable boost::asio::io_service m_io_service;
    mutable boost::asio::ip::tcp::socket m_socket;
};

//
//
//	CTcpClientConnection
//
/////////////////////////////////////////////////////////

class COMMONUTILITIES_API CTcpClientConnection {
   public:
    CTcpClientConnection(boost::asio::ip::tcp::socket& socket,
                         boost::asio::io_service& io_service)
        : m_io_service(io_service), m_socket(socket) {}

    ~CTcpClientConnection(){};

    int Write(const std::string& ip,
              std::vector<unsigned char>& writeBuffer) const;

    int Write(unsigned long ipV4,
              std::vector<unsigned char>& writeBuffer) const;

   private:
    void Connect(const std::string& ip) const;

    std::string m_port;
    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::socket& m_socket;
};

//
//
//	CTcpServerConnector
//
/////////////////////////////////////////////////////////

class COMMONUTILITIES_API CTcpServerConnector {
   public:
    CTcpServerConnector(std::string port);

    ~CTcpServerConnector();

    int Read(std::vector<unsigned char>& readBuffer, unsigned long& processID);
    int Write(unsigned long processID, std::vector<unsigned char>& writeBuffer);

    ///  callback for accepts (new tcp connections)
    void accept_handler(const boost::system::error_code& error);

    ///  open/close the listening socket for accepts
    void StartListening();
    void StopListening();

    void Stop();

    struct TcpLockPair {
        std::shared_ptr<CTcpClientConnection> tcpConnector;
        std::shared_ptr<boost::shared_mutex>
            lock;  // Lock each connection for atomic sends
    };

    // the connections map (clients connected sockets)
    std::map<uint32_t, TcpLockPair> mTcpClientsMap;

   private:
    std::string m_port;
    boost::shared_mutex m_write_lock;
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::endpoint m_endpoint;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
};

}  // namespace InuDev

#ifdef _MSC_VER
#pragma warning(default : 4251)
#endif

#endif
