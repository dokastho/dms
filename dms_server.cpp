#include <iostream>
#include <string>

#include "dms.h"
#include "drpc.h"


int main()
{
    std::string s = "";

    drpc_host dh{"localhost", 8192};

    dms_server serv(dh);
    serv.start();

    do
    {
        std::cout << "Enter 'q' to quit" << std::endl;
        std::cin >> s;
    } while (s[0] != 'q');
    
    return 0;
}