
#ifndef __LOCALSOCKETCONNECTION_H__
#define __LOCALSOCKETCONNECTION_H__

#ifdef _MSC_VER
// define windows XP
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif

#include <boost/asio/detail/config.hpp>

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS

// This definition comes to eleminate many warnings from boost of
// [-Wunused-variable]
#define BOOST_SYSTEM_NO_DEPRECATED 1

#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <string>

namespace InuCommon {

void SafeCloseSocket(int iSocket);
////////////////////////////////////////////////////////////////////////
/// \brief    Brief description, ends with empty line
///
/// Role:
/// TODO: write role
///
/// Responsibilities:
///      1.
///      2.
///
/// Comments:
///
////////////////////////////////////////////////////////////////////////
class CLocalSocketClientConnector {
   public:
    CLocalSocketClientConnector(std::string iPath);

    ~CLocalSocketClientConnector();

    size_t Write(std::vector<unsigned char>& writeBuffer) const;

    void Stop();

   private:
    sockaddr_un mAddress;
    int mAddressLength;
    int mSocket;
};

class CLocalSocketServerConnector {
   public:
    CLocalSocketServerConnector(std::string path);

    ~CLocalSocketServerConnector();

    void Write(std::vector<unsigned char>& writeBuffer) const;

    int AcceptConnection();

    void Connect() const;

    int Read(std::vector<unsigned char>& readResult, int iSocket);
    int Read(std::vector<unsigned char>& readResult);

    /// \brief    Use unsynchronized accept
    ///
    ///
    ///  callback function which call when new data arrived from client
    void accept_handler(const boost::system::error_code& error);

    /// \brief    Use for exit form tcp accept
    ///
    ///  close the Tcp socket at application exit
    ///  call from destructor of CIpcServerTCP
    void StopListening();

    void Stop();

    void Start();

    std::string GetSender();

    bool IsAlive();

   private:
    sockaddr_un mAddress;
    int mLastAcceptedSocket;
    int mAddressLength;
    int mSocket;
    bool mAcceptRecived;
};

}  // namespace InuCommon
#endif

#endif
