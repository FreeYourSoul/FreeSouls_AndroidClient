//
// Created by FyS on 23/05/17.
//

#ifndef FREESOULS_TCPCONNECTION_HH
#define FREESOULS_TCPCONNECTION_HH

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <Context.hh>

// forward declarations
namespace fys {
    namespace pb {
        class FySMessage;
    }
    namespace mq {
        template<typename T, int U>
        class FysBus;
    }
}
namespace google::protobuf {
    class Message;
}

namespace fys::network {

    /**
     * This class reprensent a connection of a client to the Server
     */
    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:
        using uptr = std::unique_ptr<TcpConnection>;
        using ptr = std::shared_ptr<TcpConnection>;
        using wptr = std::weak_ptr<TcpConnection>;

        static inline ptr create(boost::asio::io_service& io_service) {
            return std::make_shared<TcpConnection>(io_service);
        }

        explicit TcpConnection(boost::asio::io_service& io_service);

        boost::asio::ip::tcp::socket& getSocket();

        void readOnSocket(std::shared_ptr<mq::FysBus<pb::FySMessage, cl::BUS_QUEUES_SIZE>> &fysBus);
        void uniqueReadOnSocket(std::function<void(const std::string&, ushort, const std::string&)> handler);
        void send(google::protobuf::Message&& msg);

        void setCustomShutdownHandler(const std::function<void()> &customShutdownHandler);
        void setSessionIndex(uint _sessionIndex);

        const std::function<void()> &getCustomShutdownHandler() const;
        uint getSessionIndex() const;
        /**
         * \brief Get the remoge ip address
         * \return
         */
        std::string getIpAddress() const;
        /**
         * \brief Get the remote port
         * \return remote port
         */
        ushort getPort() const;


    private:
        void shuttingConnectionDown();

        void handleRead(const boost::system::error_code &error, size_t bytesTransferred,
                        std::shared_ptr<mq::FysBus<pb::FySMessage, cl::BUS_QUEUES_SIZE>>);

    private:
        bool _isShuttingDown;
        uint _sessionIndex;
        boost::asio::ip::tcp::socket _socket;

        static const int MESSAGE_BUFFER_SIZE = 512;
        mutable u_char _buffer[MESSAGE_BUFFER_SIZE];

        std::function<void ()> _customShutdownHandler;
    };

}

#endif //FREESOULS_TCPCONNECTION_HH
