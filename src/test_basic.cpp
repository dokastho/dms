#include <iostream>
#include <string>

#include "dms.h"
#include "drpc.h"
#include "test_helpers.h"


int main()
{
    DMSTester dtr;
    std::string s = "";

    drpc_client c;

    msg p{0, 8200, rand(), "localhost", "light", "on"};
    msg_reply r{1};
    rpc_arg_wrapper req{(void*)&p, sizeof(msg)};
    rpc_arg_wrapper rep{(void*)&r, sizeof(msg_reply)};

    drpc_host dh{"localhost", 8192};
    int status;
    do
    {
        r.status = 1;
        status = 0;
        status = c.Call(dh, "recv", &req, &rep);
        if (status != 0)
        {
            break;
        }
        
    } while (r.status == PENDING);
    if (status != DONE)
    {
        std::cout << "client send failed" << std::endl;
        return 1;
    }
    if (r.status != DONE)
    {
        std::cout << "client reply status failed" << std::endl;
        return 1;
    }


    std::cout << "passed" << std::endl;
    
    return 0;
}
