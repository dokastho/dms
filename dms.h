#ifndef DMS_H
#define DMS_H

#include <map>

#include "Channel.h"
#include "drpc.h"

struct msg
{
    uint8_t rank;
    int addr;
    char data[1024];
};

class dms_server
{
private:
    drpc_server *drpc_engine;
    priority_channel<msg> buffer;

    void send_worker();

public:
    dms_server(drpc_host &);

    void start();

    static void msg_receive(dms_server *, drpc_msg &);

    void msg_send(msg &);

    ~dms_server();
};

#endif