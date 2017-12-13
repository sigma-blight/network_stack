#include <iostream>
#include <memory>
#include <unistd.h>

#include <asynch/controller.hpp>
#include <meta/container/string.hpp>

class Connection : std::enable_shared_from_this<Connection>
{
    
};

class Server
{
    using conn_t = Connection;
    using conn_ptr_t = std::shared_ptr<conn_t>;
    
  public:
  
    conn_ptr_t accept()
    {
        usleep(2000000); // delay 2 seconds
        return std::make_shared<conn_t>();
    }
    
    template <typename Data_>
    void write(conn_ptr_t, Data_ && data)
    {
        
    }
};

int main()
{
    using namespace asynch;
    using namespace meta::literals;
    
    Server server;
    
    auto accept = [&](auto & controller)
    { 
        auto conn = server.accept();
        std::cout << "Accepted New Connection\n";
        controller("write"_s, conn, "Hi Cilent");
        controller("accept"_s);
    };
    
    auto write = [&](auto &, auto conn, auto data)
    {
        std::cout << "Writing Data: " << data << "\n";
        server.write(conn, data);
    };
    
    auto high_refresh = [&](auto &)
    {
        while (true)
        {
            usleep(200000); // 5 times a second
            std::cout << "-- refresh --\n";
        }
    };
    
    Controller controller
    {
        Event{ "accept"_s, accept },
        Event{ "refresh"_s, high_refresh },
        Event{ "write"_s, write }
    };
    
    controller("accept"_s);    
    controller("refresh"_s);
    
    controller.execute(2);
    
    /*  End goal output:
    
        Refresh Thread      Accept Thread
        
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
                            Accepted New Connection
                            Writing Data: Hi Client
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
        -- refresh --
                            Accepted New Connection
                            Writing Data: Hi Client
    */
}




