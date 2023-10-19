#include <iostream>
#include <string>

#include "dms.h"
#include "drpc.h"


int main()
{
    std::string s = "";

    drpc_client cli;

    msg p{0, rand(), 0, {1, 2, 3}};
    msg_reply r{1};
    rpc_arg_wrapper req{(void*)&p, sizeof(msg)};
    rpc_arg_wrapper rep{(void*)&r, sizeof(msg_reply)};

    drpc_host dh{"localhost", 8192};
    int status = cli.C
    all(dh, "recv", &req, &rep);
    if (status != 0)
    {
        std::cout << "client send failed" << std::endl;
        return 1;
    }
    if (r.status != 0)
    {
        std::cout << "client reply status failed" << std::endl;
        return 1;
    }

    std::cout << "passed" << std::endl;
    
    return 0;
}
