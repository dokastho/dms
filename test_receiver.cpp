#include <iostream>
#include <string>

#include "dms.h"
#include "drpc.h"


class Server
{
private:
    std::string status;
public:
    static void Light(Server *srv, drpc_msg &m)
    {
        std::string light_op((char*)m.req->args);
        rpc_reply *rep = (rpc_reply *)m.rep->args;
        srv->status = light_op;
        std::cout << "lights turned " << light_op << std::endl;
        rep->status = 0;
    }
};

int main()
{
    
    drpc_host dh{"localhost", 8200};

    Server srv;
    drpc_server s(dh, &srv);

    s.publish_endpoint("light", (void*)Server::Light);

    s.start();

    std::string str;
    do
    {
        std::cout << "Enter 'q' to quit: ";
        std::cin >> str;
    } while (str[0] != 'q');
    return 0;
}
