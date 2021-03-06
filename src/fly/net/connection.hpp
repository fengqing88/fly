/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    _______    _                                     *
 *                   (  ____ \  ( \     |\     /|                      * 
 *                   | (    \/  | (     ( \   / )                      *
 *                   | (__      | |      \ (_) /                       *
 *                   |  __)     | |       \   /                        *
 *                   | (        | |        ) (                         *
 *                   | )        | (____/\  | |                         *
 *                   |/         (_______/  \_/                         *
 *                                                                     *
 *                                                                     *
 *     fly is an awesome c++11 network library.                        *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-22 17:15:25                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__CONNECTION
#define FLY__NET__CONNECTION

#include <memory>
#include "fly/net/addr.hpp"
#include "fly/net/message.hpp"
#include "fly/net/message_chunk_queue.hpp"

namespace fly {
namespace net {

template<typename T>
class Poller_Executor;

template<typename T>
class Client;

template<typename T>
class Server;

template<typename T>
class Message;

template<typename T>
class Connection {};

//json protocol
template<>
class Connection<Json> : public std::enable_shared_from_this<Connection<Json>>
{
    friend class Poller_Executor<Json>;
    friend class Server<Json>;
    friend class Client<Json>;
    
public:
    Connection(int32 fd, const Addr &peer_addr);
    ~Connection();
    uint64 id();
    void close();
    bool closed();
    void send(const void *data, uint32 size);
    void send(rapidjson::Document &doc);
    const Addr& peer_addr();
    bool is_passive();
    void set_passive(bool is_passive);
    std::string key() const;
    void key(std::string k);

private:
    int32 m_fd;
    void parse();
    uint64 m_id = 0;
    uint32 m_max_msg_length = 0;
    uint32 m_cur_msg_length = 0;
    bool m_stop_parse = false;
    Addr m_peer_addr;
    bool m_is_passive;
    std::string m_key;
    std::atomic<bool> m_closed {false};
    std::shared_ptr<Connection> m_self; //add ref
    Message_Chunk_Queue m_recv_msg_queue;
    Message_Chunk_Queue m_send_msg_queue;
    Poller_Executor<Json> *m_poller_executor = nullptr;
    static fly::base::ID_Allocator m_id_allocator;
    std::function<void(std::shared_ptr<Connection>)> m_close_cb;
    std::function<void(std::shared_ptr<Connection>)> m_be_closed_cb;
    std::function<bool(std::shared_ptr<Connection>)> m_init_cb;
    std::function<void(std::unique_ptr<Message<Json>>)> m_dispatch_cb;
};

//websocket protocol
template<>
class Connection<Wsock> : public std::enable_shared_from_this<Connection<Wsock>>
{
    friend class Poller_Executor<Wsock>;
    friend class Server<Wsock>;
    friend class Client<Wsock>;
    
public:
    Connection(int32 fd, const Addr &peer_addr);
    ~Connection();
    uint64 id();
    void close();
    bool closed();
    void send(const void *data, uint32 size);
    void send(rapidjson::Document &doc);
    const Addr& peer_addr();
    bool is_passive();
    void set_passive(bool is_passive);
    std::string key() const;
    void key(std::string k);
    
private:
    void send_raw(const void *data, uint32 size);
    void parse();
    int32 m_fd;
    uint64 m_id = 0;
    uint32 m_max_msg_length = 0;
    uint8 m_cur_msg_length = 0;
    uint64 m_cur_msg_length_1 = 0;
    bool m_stop_parse = false;
    bool m_is_passive;
    bool m_handshake_phase = true;
    Addr m_peer_addr;
    std::string m_key;
    std::atomic<bool> m_closed {false};
    std::shared_ptr<Connection> m_self; //add ref
    Message_Chunk_Queue m_recv_msg_queue;
    Message_Chunk_Queue m_send_msg_queue;
    Poller_Executor<Wsock> *m_poller_executor = nullptr;
    static fly::base::ID_Allocator m_id_allocator;
    std::function<void(std::shared_ptr<Connection>)> m_close_cb;
    std::function<void(std::shared_ptr<Connection>)> m_be_closed_cb;
    std::function<bool(std::shared_ptr<Connection>)> m_init_cb;
    std::function<void(std::unique_ptr<Message<Wsock>>)> m_dispatch_cb;
};

}
}

#endif
