#include <thread>
#include <vector>
#include <iostream>
#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

#include <meta/container/string.hpp>
using namespace meta::literals;

template <typename Tag_, typename Delegate_>
struct Event
{
    using tag_t = Tag_;
    
    Delegate_ delegate;
    
    template <typename _Delegate_>
    Event(Tag_, _Delegate_ && delegate_) :
        delegate{ std::forward<_Delegate_>(delegate_) }
    {}
    
    template <typename... Args_>
    void operator() (Args_ && ... args)
    {
        delegate(std::forward<Args_>(args)...);
    }
};

template <typename Tag_, typename Delegate_>
Event(Tag_, Delegate_ &&) -> Event<Tag_, std::decay_t<Delegate_>>;

template <typename... Events_>
struct Controller : private Events_...
{    
    boost::asio::io_service io_service;

    Controller(void) = delete;
    Controller(const Controller &) = delete;
    Controller(Controller &&) = delete;
    
    Controller(Events_ ... events) :
        Events_{ events }...
    {}
    
    void execute(const size_t count)
    {
        boost::thread_group threads;
        for (unsigned i = 0; i != count; ++i)
            threads.create_thread([this]{ io_service.run(); });
        threads.join_all();
//        std::vector<std::thread> threads;
//        threads.reserve(count);
//        
//        for (unsigned i = 0; i != count; ++i)
//            threads.emplace_back([this]{ io_service.run(); });
//            
//        threads.join_all();
    }
    
    template <typename Tag_, typename... Args_>
    void operator() (Tag_, Args_ ... args)
    {
        io_service.post([=]{ get_event<Tag_, Events_...>()(*this, args...); });
    }
    
    template <typename Tag_, typename Event_, typename... Trail_>
    constexpr auto get_event()
    {
        if constexpr (std::is_same_v<Tag_, typename Event_::tag_t>)
            return static_cast<Event_&>(*this);
        else return get_event<Tag_, Trail_...>();
    }
};

struct Connection : std::enable_shared_from_this<Connection>
{
    boost::asio::ip::tcp::socket sock;
    
    Connection(boost::asio::io_service & io_service) :
        sock{ io_service }
    {}
};

struct Server
{
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acc;
    
    Server() :
        acc{ io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5001) }
    {}
     
    auto accept()
    {
        auto conn = std::make_shared<Connection>(io_service);
        acc.accept(conn->sock);
        return conn;
    }
    
    void write(std::shared_ptr<Connection> conn, std::string data)
    {
        boost::asio::write(conn->sock, boost::asio::buffer(data));
    }
    
    auto read(std::shared_ptr<Connection> conn)
    {
        boost::array<char, 128> buf;
        boost::system::error_code error;

        conn->sock.read_some(boost::asio::buffer(buf), error);
        
        return buf;
    }
};

int main()
{
    Server server;
    
    auto accept = [&](auto & controller)
    {
        auto conn = server.accept();
        std::cout << "ACCEPTED\n";
        
        controller("write"_s, conn, "Hi Client!");
        controller("read"_s, conn);
        controller("accept"_s);
    };
    
    auto write = [&](auto & controller, auto conn, auto data)
    {
        std::cout << "Write: " << data << "\n";
        server.write(conn, data);
    };    
    
    auto read = [&](auto & controller, auto conn)
    {
        auto data = server.read(conn);
        
        std::cout << "Read: ";
        for (auto & x : data) { if (x == '\n') break; std::cout << x; }
        std::cout << "\n";
        
        controller("read"_s, conn);
        controller("write"_s, conn, "Got your message!");
    };
    
    auto ref = [](auto &)
    {
        while(true)
        {
            usleep(500000);
            std::cout << "-- refresh -- \n";
        }
    };
    
    Controller controller
    { 
        Event{ "accept"_s, accept }, 
        Event{ "ref"_s, ref }, 
        Event{ "write"_s, write },
        Event{ "read"_s, read }
    };    
    
    controller("accept"_s);  
    controller("ref"_s);
    controller.execute(4);
}




