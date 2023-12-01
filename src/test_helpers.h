#ifndef DMS_TEST_HELPERS
#define DMS_TEST_HELPERS

#include <string>
#include <iostream>

#include "drpc.h"
#include "dms.h"

class __TestReceiver
{
private:
    std::string status;
    drpc_server *drpc_engine;

public:
    __TestReceiver()
    {
        drpc_host dh{"localhost", 8200};
        drpc_engine = new drpc_server(dh, this);

        drpc_engine->publish_endpoint("light", (void *)__TestReceiver::Light);

        drpc_engine->start();
    }
    static void Light(__TestReceiver *srv, drpc_msg &m)
    {
        std::string light_op((char *)m.req->args);
        rpc_reply *rep = (rpc_reply *)m.rep->args;
        srv->status = light_op;
        std::cout << "lights turned " << light_op << std::endl;
        rep->status = 0;
    }
    ~__TestReceiver()
    {
        delete drpc_engine;
    }
};

class __TestDMS
{
private:
    dms_server *dms_engine;

public:
    __TestDMS()
    {
        drpc_host dh{"localhost", 8192};
        dms_engine = new dms_server(dh);
        dms_engine->start();
    }
    ~__TestDMS()
    {
        delete dms_engine;
    }
};

class DMSTester
{
    __TestReceiver tr;
    __TestDMS tdms;
};

#endif